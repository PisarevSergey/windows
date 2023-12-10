#include "pch.h"

namespace {
}

NTSTATUS UsbFilterDeviceContext::SetInstanceId(WDFDEVICE device) {
    NTSTATUS status{};
    auto deviceIdMemory = kmdf::AllocAndQueryDeviceProperty(status, device, DEVPKEY_Device_InstanceId, device);
    if (status != STATUS_SUCCESS) {
        TraceError("kmdf::AllocAndQueryDeviceProperty failed to get DEVPKEY_Device_InstanceId", TraceLoggingNTStatus(status));
        return status;
    }

    m_instanceId = helpers::ToStringView(status, deviceIdMemory);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    return STATUS_SUCCESS;
}
