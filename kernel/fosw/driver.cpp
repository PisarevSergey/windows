#include "pch.h"

#include <kcpp/scope_guard.h>

TRACELOGGING_DEFINE_PROVIDER(g_tracer,
    "FoSwLoggingProviderKM",
    (0x5d6e68d6, 0xd2b7, 0x4354, 0x81, 0x95, 0x30, 0xa8, 0x7c, 0x18, 0x6a, 0xad));

namespace {

    NTSTATUS Unload([[maybe_unused]] FLT_FILTER_UNLOAD_FLAGS Flags) {
        TraceInfo("unloading");

        g_driver.~Driver();
        return STATUS_SUCCESS;
    }

    NTSTATUS Attach(PCFLT_RELATED_OBJECTS fltObjects,
        FLT_INSTANCE_SETUP_FLAGS flags,
        DEVICE_TYPE volumeDeviceType,
        FLT_FILESYSTEM_TYPE volumeFilesystemType) {

        if (!FlagOn(flags, FLTFL_INSTANCE_SETUP_NEWLY_MOUNTED_VOLUME)) {
            TraceVerbose("not a newly mounted volume, won't attach");
            return STATUS_FLT_DO_NOT_ATTACH;
        }

        if (FILE_DEVICE_DISK_FILE_SYSTEM != volumeDeviceType) {
            TraceVerbose("not a disk volume, won't attach");
            return STATUS_FLT_DO_NOT_ATTACH;
        }

        if (FLT_FSTYPE_RAW == volumeFilesystemType) {
            TraceVerbose("RAW file system mounted, won't attach");
            return STATUS_FLT_DO_NOT_ATTACH;
        }

        {
            BOOLEAN volumeIsWritable{};
            const auto status = FltIsVolumeWritable(fltObjects->Volume, &volumeIsWritable);
            if (!NT_SUCCESS(status)) {
                TraceError("FltIsVolumeWritable failed", TraceLoggingNTStatus(status));
                return status;
            }

            if (FALSE == volumeIsWritable) {
                TraceVerbose("volume is write protected, won't attach");
                return STATUS_FLT_DO_NOT_ATTACH;
            }
        }

        {
            BOOLEAN volumeIsSnapshot{};
            const auto status = FltIsVolumeSnapshot(fltObjects->Volume, &volumeIsSnapshot);
            if (!NT_SUCCESS(status)) {
                TraceError("FltIsVolumeSnapshot failed", TraceLoggingNTStatus(status));
                return status;
            }

            if (TRUE == volumeIsSnapshot) {
                TraceVerbose("snapshot volume, won't attach");
                return STATUS_FLT_DO_NOT_ATTACH;
            }
        }

        return STATUS_SUCCESS;
    }

    alignas(Driver) char driverMemory[sizeof(Driver)];
}

Driver& g_driver = *reinterpret_cast<Driver*>(driverMemory);

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT driverObject, [[maybe_unused]] PUNICODE_STRING regPath) {

    new(&g_driver) Driver;
    auto driverGuard = kcpp::make_scope_guard([] { g_driver.~Driver(); } );

    auto status = g_driver.Register(*driverObject);
    if (!NT_SUCCESS(status)) {
        TraceError("driver registration failed", TraceLoggingNTStatus(status));
        return status;
    }

    status = g_driver.Start();
    if (!NT_SUCCESS(status)) {
        TraceError("failed to start filtering", TraceLoggingNTStatus(status));
        return status;
    }

    TraceInfo("driver start success");
    driverGuard.release();
    return STATUS_SUCCESS;
}

NTSTATUS Driver::Register(DRIVER_OBJECT& driverObject) {

    constexpr FLT_REGISTRATION fltReg {
        .Size = sizeof(fltReg),
        .Version = FLT_REGISTRATION_VERSION,
        .FilterUnloadCallback = Unload,
        .InstanceSetupCallback = Attach
    };

    NTSTATUS status{};
    m_minifilter = flt::CreateMinifilter(status, driverObject, fltReg);

    return status;
}

NTSTATUS Driver::Start() {
    return FltStartFiltering(m_minifilter.get());
}
