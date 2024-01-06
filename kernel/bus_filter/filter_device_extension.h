#pragma once

class FilterDeviceExtension final {
public:
    FilterDeviceExtension() = default;
    ~FilterDeviceExtension() = default;

    NTSTATUS Attach(DEVICE_OBJECT& filterDevice, DEVICE_OBJECT& pdo);

    NTSTATUS DispatchPnp(IRP& irp);

    NTSTATUS ForwardAndForget(IRP& irp);

    void ReportIoctl(IRP& irp) const;
    void ReportInternalIoctl(IRP& irp) const;

    void OnDeviceUsageNotificationComplete(DEVICE_OBJECT& deviceObject, PIRP irp);

    FilterDeviceExtension(FilterDeviceExtension&&) = delete;

    void* operator new(size_t, void* p) { return p; }
private:
    nt::RemoveLock m_removeLock{ 'tlFB', 1, 100 };
    PDEVICE_OBJECT  m_lowerDevice{};
    bool m_interestingDevice{false};

    NTSTATUS ForwardAndForgetNoRemoveLock(IRP& irp);

    void DispatchBusRelations(const DEVICE_RELATIONS& busRelations);

    void OnStart();
};
