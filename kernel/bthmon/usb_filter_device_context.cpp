#include "pch.h"

namespace {
}

UsbFilterDeviceContext::UsbFilterDeviceContext() : LIST_ENTRY{nullptr, nullptr}
{}

UsbFilterDeviceContext::~UsbFilterDeviceContext() {
    Flink = Blink = nullptr;
}

NTSTATUS UsbFilterDeviceContext::Init(WDFDEVICE device)
{
    return SetInstanceId(device);
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

void UsbFilters::Insert(UsbFilterDeviceContext& context)
{
    kcpp::lock_guard<nt::FastMutex> guard{ m_lock };
    m_filters.InsertTail(context);
}

void UsbFilters::Remove(UsbFilterDeviceContext& context)
{
    kcpp::lock_guard<nt::FastMutex> guard{ m_lock };
    m_filters.Remove(context);
}

void UsbFilters::StartFilterUsbDevice(const UNICODE_STRING& deviceId)
{
    kcpp::lock_guard<nt::FastMutex> guard{ m_lock };

    for (auto& currentUsbDeviceContext : m_filters) {
        if (RtlEqualUnicodeString(&deviceId, &currentUsbDeviceContext.GetInstanceId(), TRUE)) {
            currentUsbDeviceContext.SetPassthrough(false);
            return;
        }
    }
}
