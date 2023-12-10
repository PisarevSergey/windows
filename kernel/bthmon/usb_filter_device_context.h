#pragma once

class UsbFilterDeviceContext final {
public:
    bool IsPassthrough() const { return m_passthrough; }
    void SetPassthrough(bool passthrough) { m_passthrough = passthrough; }

    [[nodiscard]]
    NTSTATUS SetInstanceId(WDFDEVICE device);

    const UNICODE_STRING& GetInstanceId() const { return m_instanceId; }

    UsbFilterDeviceContext() = default;
    ~UsbFilterDeviceContext() = default;

    void* operator new(size_t, void* p) { return p; }

    UsbFilterDeviceContext(UsbFilterDeviceContext&&) = delete;
private:
    UNICODE_STRING m_instanceId{};
    bool m_passthrough{ true };
};

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(UsbFilterDeviceContext, GetUsbFilterContext);
