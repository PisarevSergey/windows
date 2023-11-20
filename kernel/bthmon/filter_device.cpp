#include "pch.h"

namespace {
    void ForwardAndForget(WDFREQUEST request, WDFQUEUE queue) {
        auto device = WdfIoQueueGetDevice(queue);
        auto ioTarget = WdfDeviceGetIoTarget(device);
        const auto status = kmdf::ForwardAndForget(request, ioTarget);
        if (!NT_SUCCESS(status))
        {
            TraceError("Failed to forward request", TraceLoggingPointer(request), TraceLoggingNTStatus(status));
            WdfRequestComplete(request, status);
        }
    }

    void OnIoctl(WDFQUEUE queue,
        WDFREQUEST request,
        [[maybe_unused]] size_t OutputBufferLength,
        [[maybe_unused]] size_t InputBufferLength,
        ULONG IoControlCode) {
        TraceInfo("ioctl", TraceLoggingHexULong(IoControlCode));
        ForwardAndForget(request, queue);
    }

    void OnInternalIoctl(WDFQUEUE queue,
        WDFREQUEST request,
        [[maybe_unused]] size_t OutputBufferLength,
        [[maybe_unused]] size_t InputBufferLength,
        ULONG IoControlCode){
        TraceInfo("internal ioctl", TraceLoggingHexULong(IoControlCode));
        ForwardAndForget(request, queue);
    }
}

NTSTATUS AddDevice([[maybe_unused]] WDFDRIVER driver, PWDFDEVICE_INIT deviceInit) {
    WdfFdoInitSetFilter(deviceInit);

    WDFDEVICE filterDevice{};
    auto status = WdfDeviceCreate(&deviceInit, WDF_NO_OBJECT_ATTRIBUTES, &filterDevice);
    if (STATUS_SUCCESS != status)
    {
        TraceError("WdfDeviceCreate failed to create filter device", TraceLoggingNTStatus(status));
        return status;
    }

    GUID busTypeGuid{};
    ULONG returnLength{};
    status = WdfDeviceQueryProperty(filterDevice, DevicePropertyBusTypeGuid, sizeof(busTypeGuid), &busTypeGuid, &returnLength);
    if (!NT_SUCCESS(status))
    {
        TraceError("WdfDeviceQueryProperty failed to get DevicePropertyBusTypeGuid", TraceLoggingNTStatus(status));
        return status;
    }

    if (GUID_BUS_TYPE_USB != busTypeGuid)
    {
        TraceInfo("do not attach to non-root bluetooth device");
        return STATUS_FLT_DO_NOT_ATTACH;
    }

    auto queueConfig = kmdf::CreateQueueConfig();
    queueConfig.EvtIoDeviceControl = OnIoctl;
    queueConfig.EvtIoInternalDeviceControl = OnInternalIoctl;
    status = WdfIoQueueCreate(filterDevice, &queueConfig, WDF_NO_OBJECT_ATTRIBUTES, WDF_NO_HANDLE);
    if (STATUS_SUCCESS != status)
    {
        TraceError("WdfIoQueueCreate failed", TraceLoggingNTStatus(status));
        return status;
    }

    return STATUS_SUCCESS;
}
