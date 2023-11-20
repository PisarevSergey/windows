#include "pch.h"

TRACELOGGING_DEFINE_PROVIDER(g_tracer, "BthMonLoggingProviderKM", (0x0776adb6, 0x9c11, 0x4cce, 0x82, 0xbd, 0x73, 0x43, 0x80, 0xf6, 0x75, 0x22));

namespace
{
    alignas(Driver) char driverMemory[sizeof(Driver)];

    void DriverCleanup([[maybe_unused]] WDFOBJECT driverObject) {
        TraceInfo("driver cleanup");
        g_driver.~Driver();
    }

    void DriverUnload([[maybe_unused]] WDFDRIVER driver) {
        TraceInfo("driver unload");
    }

    [[nodiscard]]
    NTSTATUS CreateWdfDriver(DRIVER_OBJECT& driverObject, const UNICODE_STRING& regPath)
    {
        auto driverObjectAttributes = kmdf::CreateObjectAttributes(nullptr, DriverCleanup);
        auto driverConfig = kmdf::CreateDriverConfig(DriverUnload, AddDevice);
        return WdfDriverCreate(&driverObject, &regPath, &driverObjectAttributes, &driverConfig, WDF_NO_HANDLE);
    }
}

Driver& g_driver = *reinterpret_cast<Driver*>(driverMemory);

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT driverObject, PUNICODE_STRING regPath) {
    new (&g_driver) Driver;
    const auto status = CreateWdfDriver(*driverObject, *regPath);
    if (!NT_SUCCESS(status))
    {
        TraceError("CreateWdfDriver failed", TraceLoggingNTStatus(status));
        g_driver.~Driver();
        return status;
    }

    TraceInfo("driver initialization success");
    return STATUS_SUCCESS;
}
