#pragma once

class FilterDeviceExtension final {
public:
    FilterDeviceExtension() = default;
    ~FilterDeviceExtension() = default;

    NTSTATUS Attach(DEVICE_OBJECT& filterDevice, DEVICE_OBJECT& pdo);

    NTSTATUS DispatchPnp(IRP& irp);

    NTSTATUS ForwardAndForget(IRP& irp);

    void OnDeviceUsageNotificationComplete(DEVICE_OBJECT& deviceObject, PIRP irp);

    FilterDeviceExtension(FilterDeviceExtension&&) = delete;

    void* operator new(size_t, void* p) { return p; }
private:
    nt::RemoveLock m_removeLock{ 'tlFB', 1, 100 };
    PDEVICE_OBJECT  m_lowerDevice{};

    NTSTATUS ForwardAndForgetNoLock(IRP& irp);
};
