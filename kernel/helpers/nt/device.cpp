#include "pch.h"

void nt::DeviceDeleter::operator()(DEVICE_OBJECT* device) const
{
    IoDeleteDevice(device);
}

nt::AutoDevice nt::CreateDevice(NTSTATUS& status,
    DRIVER_OBJECT& driver,
    DEVICE_TYPE deviceType,
    ULONG deviceCharacteristics,
    BOOLEAN exclusive,
    ULONG deviceExtensionSize,
    const wchar_t* deviceName)
{
    UNICODE_STRING deviceNameUs{};
    if (deviceName)
    {
        RtlInitUnicodeString(&deviceNameUs, deviceName);
    }

    PDEVICE_OBJECT device{};
    status = IoCreateDevice(&driver,
        deviceExtensionSize,
        deviceName ? &deviceNameUs : nullptr,
        deviceType,
        deviceCharacteristics,
        exclusive,
        &device);
    if (!NT_SUCCESS(status))
    {
        TraceLoggingWrite(g_tracer,
            "failed to create device",
            TraceLoggingLevel(WINEVENT_LEVEL_ERROR),
            TraceLoggingNTStatus(status));
        return AutoDevice{};
    }

    return AutoDevice{ device };
}
