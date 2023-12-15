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

NTSTATUS AddDevice(__in PDRIVER_OBJECT DriverObject, __in PDEVICE_OBJECT PhysicalDeviceObject)
{
    const auto deviceType = [PhysicalDeviceObject]{
        auto highestDeviceObject = IoGetAttachedDeviceReference(PhysicalDeviceObject);
        const auto deviceType = highestDeviceObject->DeviceType;
        ObDereferenceObject(highestDeviceObject);
        return deviceType;}();

    PDEVICE_OBJECT filterDevice{};
    auto status = IoCreateDevice(DriverObject,
        sizeof(FilterDeviceExtension),
        nullptr,  // No Name
        deviceType,
        FILE_DEVICE_SECURE_OPEN,
        FALSE,
        &filterDevice);
    if (!NT_SUCCESS(status)) {
        //
        // Returning failure here prevents the entire stack from functioning,
        // but most likely the rest of the stack will not be able to create
        // device objects either, so it is still OK.
        //
        return status;
    }

    auto filterDeviceExtension = new(filterDevice->DeviceExtension)FilterDeviceExtension;
    status = filterDeviceExtension->Attach(*filterDevice, *PhysicalDeviceObject);
    if (!NT_SUCCESS(status)) {
        IoDeleteDevice(filterDevice);
        return status;
    }

    return STATUS_SUCCESS;
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
    new (&g_driver) Driver;

    for (auto& majorFunction : driverObject->MajorFunction) {
        majorFunction = DefaultDispatch;
    }

    driverObject->MajorFunction[IRP_MJ_PNP] = DispatchPnp;
    driverObject->DriverExtension->AddDevice = AddDevice;
    driverObject->DriverUnload = Unload;

    TraceInfo("driver loaded");

    return STATUS_SUCCESS;
}
