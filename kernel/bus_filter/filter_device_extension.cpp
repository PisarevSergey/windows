#include "pch.h"

#include <kcpp/scope_guard.h>

static NTSTATUS FilterDeviceUsageNotificationCompletionRoutine(PDEVICE_OBJECT deviceObject,
    PIRP irp,
    [[maybe_unused]] PVOID context) {

    if (irp->PendingReturned) {
        IoMarkIrpPending(irp);
    }

    auto deviceExtension = static_cast<FilterDeviceExtension*>(deviceObject->DeviceExtension);
    deviceExtension->OnDeviceUsageNotificationComplete(*deviceObject, irp);

    return STATUS_CONTINUE_COMPLETION;
}

NTSTATUS FilterDeviceExtension::Attach(DEVICE_OBJECT& filterDevice, DEVICE_OBJECT& pdo) {

    const auto status = IoAttachDeviceToDeviceStackSafe(&filterDevice, &pdo, &m_lowerDevice);
    if (!NT_SUCCESS(status)) {
        TraceError("IoAttachDeviceToDeviceStackSafe failed", TraceLoggingNTStatus(status));
        return status;
    }

    filterDevice.Flags |= m_lowerDevice->Flags & (DO_BUFFERED_IO | DO_DIRECT_IO | DO_POWER_PAGABLE);


    filterDevice.DeviceType = m_lowerDevice->DeviceType;
    filterDevice.Characteristics = m_lowerDevice->Characteristics;

    //
    // Set the initial state of the Filter DO
    //

    filterDevice.Flags &= ~DO_DEVICE_INITIALIZING;

    return STATUS_SUCCESS;
}

NTSTATUS FilterDeviceExtension::DispatchPnp(IRP& irp)
{

    auto status = m_removeLock.Acquire(&irp);
    if (!NT_SUCCESS(status)) {
        TraceError("failed to acquire remove lock", TraceLoggingNTStatus(status));
        return nt::Complete(irp, status);
    }
    auto removeLockGuard = kcpp::scope_guard{ [this, &irp] {m_removeLock.Release(&irp); } };

    const auto& irpStack = *IoGetCurrentIrpStackLocation(&irp);
    auto filterDeviceObject = irpStack.DeviceObject;

    switch (irpStack.MinorFunction) {
    case IRP_MN_START_DEVICE:
        TraceInfo("starting device", TraceLoggingPointer(filterDeviceObject));
        //
        // The device is starting.
        // We cannot touch the device (send it any non pnp irps) until a
        // start device has been passed down to the lower drivers.
        //

        if (IoForwardIrpSynchronously(m_lowerDevice, &irp)) {

            status = irp.IoStatus.Status;
            if (NT_SUCCESS(status)) {
                //
                // On the way up inherit FILE_REMOVABLE_MEDIA during Start.
                // This characteristic is available only after the driver stack is started!.
                //
                if (m_lowerDevice->Characteristics & FILE_REMOVABLE_MEDIA) {
                    filterDeviceObject->Characteristics |= FILE_REMOVABLE_MEDIA;
                }

                OnStart();
            }

            return nt::Complete(irp, status);
        }

        TraceCritical("IoForwardIrpSynchronously failed");

        break;
    case IRP_MN_REMOVE_DEVICE:
        TraceInfo("removing device", TraceLoggingPointer(filterDeviceObject));

        //
        // Wait for all outstanding requests to complete
        //
        removeLockGuard.release();
        m_removeLock.ReleaseAndWait(&irp);

        status = ForwardAndForgetNoRemoveLock(irp);

        IoDetachDevice(m_lowerDevice);
        this->~FilterDeviceExtension();
        IoDeleteDevice(filterDeviceObject);

        return status;

    case IRP_MN_DEVICE_USAGE_NOTIFICATION:

        //
        // On the way down, pagable might become set. Mimic the driver
        // above us. If no one is above us, just set pagable.
        //
#pragma prefast(suppress:__WARNING_INACCESSIBLE_MEMBER)
        if ((filterDeviceObject->AttachedDevice == NULL) ||
            (filterDeviceObject->AttachedDevice->Flags & DO_POWER_PAGABLE)) {

            filterDeviceObject->Flags |= DO_POWER_PAGABLE;
        }

        return nt::SendWithCompletionRoutine(*m_lowerDevice, irp, FilterDeviceUsageNotificationCompletionRoutine);

    case IRP_MN_QUERY_DEVICE_RELATIONS:

        if (irpStack.Parameters.QueryDeviceRelations.Type == BusRelations) {
            TraceInfo("query bus relations", TraceLoggingPointer(filterDeviceObject));

            if (IoForwardIrpSynchronously(m_lowerDevice, &irp)) {

                if (NT_SUCCESS(irp.IoStatus.Status))
                {
                    DispatchBusRelations(*reinterpret_cast<DEVICE_RELATIONS*>(irp.IoStatus.Information));
                }

                return nt::Complete(irp, irp.IoStatus.Status);
            }

            TraceCritical("IoForwardIrpSynchronously failed");
        }
        break;
    }

    return ForwardAndForgetNoRemoveLock(irp);
}

NTSTATUS FilterDeviceExtension::ForwardAndForget(IRP& irp) {

    const auto status = m_removeLock.Acquire(&irp);
    if (!NT_SUCCESS(status)) {
        return nt::Complete(irp, status);
    }
    KCPP_SCOPE_GUARD(kcpp::make_scope_guard([this, &irp] {m_removeLock.Release(&irp); }));

    return ForwardAndForgetNoRemoveLock(irp);
}

void FilterDeviceExtension::ReportIoctl(IRP& irp) const {

    if (m_interestingDevice) {
        const auto& stackLocation = *IoGetCurrentIrpStackLocation(&irp);
        NT_ASSERT(stackLocation.MajorFunction == IRP_MJ_DEVICE_CONTROL);

        TraceInfo("ioctl");
    }
}

void FilterDeviceExtension::ReportInternalIoctl(IRP& irp) const {

    if (m_interestingDevice) {
        const auto& stackLocation = *IoGetCurrentIrpStackLocation(&irp);
        NT_ASSERT(stackLocation.MajorFunction == IRP_MJ_INTERNAL_DEVICE_CONTROL);

        if (IOCTL_INTERNAL_USB_SUBMIT_URB == stackLocation.Parameters.DeviceIoControl.IoControlCode) {
            Report(*static_cast<URB*>(URB_FROM_IRP(&irp)));
        }
    }
}

void FilterDeviceExtension::OnDeviceUsageNotificationComplete(DEVICE_OBJECT& deviceObject, PIRP irp) {

    KCPP_SCOPE_GUARD(kcpp::make_scope_guard([this, irp] { m_removeLock.Release(irp); }));

    // On the way up, pagable might become clear.
    // Mimic the driver below us.
    if (!(m_lowerDevice->Flags & DO_POWER_PAGABLE)) {

        deviceObject.Flags &= ~DO_POWER_PAGABLE;
    }
}

NTSTATUS FilterDeviceExtension::ForwardAndForgetNoRemoveLock(IRP& irp) {
    IoSkipCurrentIrpStackLocation(&irp);
    return IoCallDriver(m_lowerDevice, &irp);
}

void FilterDeviceExtension::DispatchBusRelations([[maybe_unused]] const DEVICE_RELATIONS& busRelations)
{
    for (ULONG i{ 0 }; i != busRelations.Count; ++i) {
        auto& dev = *busRelations.Objects[i];

        const auto status = g_driver.Attach(dev);
        if (NT_SUCCESS(status)) {
            TraceInfo("attached to device", TraceLoggingPointer(&dev));
        }
        else {
            TraceError("failed to attach",
                TraceLoggingPointer(&dev),
                TraceLoggingNTStatus(status));
        }

    }
}

void FilterDeviceExtension::OnStart()
{
    const auto bthUsbIsInStack = [this] {

        for (nt::AutoReferencedDevice currentDevice{ IoGetAttachedDeviceReference(m_lowerDevice) };
            currentDevice.get() != nullptr;
            currentDevice = nt::AutoReferencedDevice{ IoGetLowerDeviceObject(currentDevice.get()) }) {

            constexpr UNICODE_STRING bthUsbDriverName = RTL_CONSTANT_STRING(LR"(\Driver\BTHUSB)");
            if (RtlEqualUnicodeString(&bthUsbDriverName, &currentDevice->DriverObject->DriverName, TRUE)) {
                return true;
            }
        }

        return false;
    }();

    if (!bthUsbIsInStack) {
        TraceInfo("no BTHUSB in stack, won't trace the device");
        return;
    }
    TraceInfo("BTHUSB is in stack");

    nt::AutoReferencedDevice pdo{ IoGetDeviceAttachmentBaseRef(m_lowerDevice) };
    GUID busType{};
    ULONG requiredSize{};
    DEVPROPTYPE propertyType{};
    const auto status = IoGetDevicePropertyData(pdo.get(), &DEVPKEY_Device_BusTypeGuid, LOCALE_NEUTRAL, 0, sizeof(busType), &busType, &requiredSize, &propertyType);
    if (!NT_SUCCESS(status)) {
        TraceError("failed to get DEVPKEY_Device_BusTypeGuid", TraceLoggingNTStatus(status));
        return;
    }
    NT_ASSERT(DEVPROP_TYPE_GUID == propertyType);
    TraceInfo("Bus type", TraceLoggingGuid(busType));

    if (GUID_BUS_TYPE_USB == busType) {
        TraceInfo("device is on the USB bus, will trace it");
        m_interestingDevice = true;
    }

}
