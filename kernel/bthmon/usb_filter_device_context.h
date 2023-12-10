#pragma once

#include <nt/list.h>

class UsbFilterDeviceContext final : public LIST_ENTRY {
public:
    UsbFilterDeviceContext();
    ~UsbFilterDeviceContext();

    bool IsPassthrough() const { return m_passthrough; }
    void SetPassthrough(bool passthrough) { m_passthrough = passthrough; }

    [[nodiscard]]
    NTSTATUS Init(WDFDEVICE device);

    const UNICODE_STRING& GetInstanceId() const { return m_instanceId; }

    void* operator new(size_t, void* p) { return p; }
    void operator delete(void*, size_t) { NT_ASSERT(FALSE); }

    UsbFilterDeviceContext(UsbFilterDeviceContext&&) = delete;
private:
    UNICODE_STRING m_instanceId{};
    bool m_passthrough{ true };

    [[nodiscard]]
    NTSTATUS SetInstanceId(WDFDEVICE device);
};

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(UsbFilterDeviceContext, GetUsbFilterContext);

using UsbFiltersList = nt::List<UsbFilterDeviceContext>;

class UsbFilters final {
public:
    UsbFilters() = default;

    void Insert(UsbFilterDeviceContext& context);
    void Remove(UsbFilterDeviceContext& context);
    void StartFilterUsbDevice(const UNICODE_STRING& deviceId);

    UsbFilters(UsbFilters&&) = delete;
private:
    nt::FastMutex m_lock;
    UsbFiltersList m_filters;
};
