#include "pch.h"

namespace {

NTSTATUS DefaultDispatch(PDEVICE_OBJECT deviceObject, PIRP irp) {
    NT_ASSERT(irp != nullptr);

    auto deviceExtension = static_cast<FilterDeviceExtension*>(deviceObject->DeviceExtension);
    return deviceExtension->ForwardAndForget(*irp);
}

NTSTATUS DispatchPnp(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    return static_cast<FilterDeviceExtension*>(DeviceObject->DeviceExtension)->DispatchPnp(*Irp);
}

NTSTATUS AddDevice([[maybe_unused]] __in PDRIVER_OBJECT DriverObject,
    __in PDEVICE_OBJECT PhysicalDeviceObject)
{
    return g_driver.Attach(*PhysicalDeviceObject);
}

void Unload([[maybe_unused]] __in PDRIVER_OBJECT driverObject)
{
    g_driver.~Driver();
}

alignas(Driver) char driverMemory[sizeof(Driver)];

}

Driver& g_driver = *reinterpret_cast<Driver*>(driverMemory);

TRACELOGGING_DEFINE_PROVIDER(g_tracer,
    "BusFilterLoggingProviderKM",
    (0xf52040da, 0x981d, 0x40e6, 0xab, 0x4e, 0x19, 0x7d, 0x55, 0xbf, 0x86, 0x08));

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT driverObject, [[maybe_unused]] PUNICODE_STRING regPath) {
    new (&g_driver) Driver{driverObject};

    for (auto& majorFunction : driverObject->MajorFunction) {
        majorFunction = DefaultDispatch;
    }

    driverObject->MajorFunction[IRP_MJ_PNP] = DispatchPnp;
    driverObject->DriverExtension->AddDevice = AddDevice;
    driverObject->DriverUnload = Unload;

    TraceInfo("driver loaded");

    return STATUS_SUCCESS;
}

Driver::Driver(PDRIVER_OBJECT driverObject) : m_driverObject{driverObject}
{}

NTSTATUS Driver::Attach(DEVICE_OBJECT& device) {

    if (AreWeInStackFor(device)) {
        TraceInfo("already in stack for device, won't attach", TraceLoggingPointer(&device));
        return STATUS_SUCCESS;
    }

    const auto deviceType = [&device] {
        auto highestDeviceObject = IoGetAttachedDeviceReference(&device);
        const auto deviceType = highestDeviceObject->DeviceType;
        ObDereferenceObject(highestDeviceObject);
        return deviceType; }();

        NTSTATUS status{};
        auto filterDevice = nt::CreateDevice(status,
            *const_cast<PDRIVER_OBJECT>(m_driverObject),
            deviceType,
            FILE_DEVICE_SECURE_OPEN,
            FALSE,
            sizeof(FilterDeviceExtension));
        if (!NT_SUCCESS(status)) {
            //
            // Returning failure here prevents the entire stack from functioning,
            // but most likely the rest of the stack will not be able to create
            // device objects either, so it is still OK.
            //
            return status;
        }

        using DeviceExtensionDestroyer = decltype([](FilterDeviceExtension* ex) {ex->~FilterDeviceExtension(); });
        using AutoDeviceExtension = kcpp::auto_ptr < FilterDeviceExtension, DeviceExtensionDestroyer >;

        AutoDeviceExtension filterDeviceExtension{ new(filterDevice->DeviceExtension)FilterDeviceExtension };

        status = filterDeviceExtension->Attach(*filterDevice, device);
        if (!NT_SUCCESS(status)) {
            return status;
        }

        KCPP_UNUSED(filterDeviceExtension.release());
        KCPP_UNUSED(filterDevice.release());

        return STATUS_SUCCESS;
}

bool Driver::AreWeInStackFor(const DEVICE_OBJECT& device) const
{
    for (nt::AutoReferencedDevice currentDevice{ IoGetAttachedDeviceReference(const_cast<PDEVICE_OBJECT>(&device))};
        currentDevice.get() != nullptr;
        currentDevice = nt::AutoReferencedDevice{ IoGetLowerDeviceObject(currentDevice.get()) }) {

        if (m_driverObject == currentDevice->DriverObject) {
            return true;
        }
    }

    return false;
}
