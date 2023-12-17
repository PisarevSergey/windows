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
    switch (irpStack.MinorFunction) {
    case IRP_MN_START_DEVICE:
        TraceInfo("starting device", TraceLoggingPointer(irpStack.DeviceObject));
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
                    irpStack.DeviceObject->Characteristics |= FILE_REMOVABLE_MEDIA;
                }
            }

            return nt::Complete(irp, status);
        }

        TraceCritical("IoForwardIrpSynchronously failed");

        break;
    case IRP_MN_REMOVE_DEVICE:
        TraceInfo("removing device", TraceLoggingPointer(irpStack.DeviceObject));

        //
        // Wait for all outstanding requests to complete
        //
        removeLockGuard.release();
        m_removeLock.ReleaseAndWait(&irp);

        status = ForwardAndForgetNoRemoveLock(irp);

        IoDetachDevice(m_lowerDevice);
        this->~FilterDeviceExtension();
        IoDeleteDevice(irpStack.DeviceObject);

        return status;

    case IRP_MN_DEVICE_USAGE_NOTIFICATION:

        //
        // On the way down, pagable might become set. Mimic the driver
        // above us. If no one is above us, just set pagable.
        //
#pragma prefast(suppress:__WARNING_INACCESSIBLE_MEMBER)
        if ((irpStack.DeviceObject->AttachedDevice == NULL) ||
            (irpStack.DeviceObject->AttachedDevice->Flags & DO_POWER_PAGABLE)) {

            irpStack.DeviceObject->Flags |= DO_POWER_PAGABLE;
        }

        return nt::SendWithCompletionRoutine(*m_lowerDevice, irp, FilterDeviceUsageNotificationCompletionRoutine);

    case IRP_MN_QUERY_DEVICE_RELATIONS:
        TraceVerbose("IRP_MN_QUERY_DEVICE_RELATIONS");

        if (irpStack.Parameters.QueryDeviceRelations.Type == BusRelations) {
            TraceInfo("query bus relations", TraceLoggingPointer(irpStack.DeviceObject));

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

void FilterDeviceExtension::OnDeviceUsageNotificationComplete(DEVICE_OBJECT& deviceObject,
    PIRP irp) {

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
