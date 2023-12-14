#include "pch.h"

#include <kcpp/scope_guard.h>

namespace {

    NTSTATUS FilterStartCompletionRoutine([[maybe_unused]] PDEVICE_OBJECT deviceObject,
            PIRP irp,
            PVOID context)
    {
        //
        // If the lower driver didn't return STATUS_PENDING, we don't need to 
        // set the event because we won't be waiting on it. 
        // This optimization avoids grabbing the dispatcher lock, and improves perf.
        //
        if (irp->PendingReturned == TRUE) {
            static_cast<nt::Event*>(context)->Set();
        }

        //
        // The dispatch routine will have to call IoCompleteRequest
        //

        return STATUS_MORE_PROCESSING_REQUIRED;
    }

    NTSTATUS FilterDeviceUsageNotificationCompletionRoutine(PDEVICE_OBJECT deviceObject,
        PIRP irp,
        PVOID context);

class FilterDeviceExtension final {
public:
    FilterDeviceExtension() = default;
    ~FilterDeviceExtension() = default;

    NTSTATUS Attach(DEVICE_OBJECT& filterDevice, DEVICE_OBJECT& pdo) {
        const auto status = IoAttachDeviceToDeviceStackSafe(&filterDevice, &pdo, &m_lowerDevice);

        //
        // Failure for attachment is an indication of a broken plug & play system.
        //
        if (!NT_SUCCESS(status)) {
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

    NTSTATUS DispatchPnp(IRP& irp)
    {
        auto status = m_removeLock.Acquire(&irp);
        if (!NT_SUCCESS(status)) {
            return nt::Complete(irp, status);
        }
        auto removeLockGuard = kcpp::scope_guard{ [this, &irp] {m_removeLock.Release(&irp); } };

        const auto& irpStack = *IoGetCurrentIrpStackLocation(&irp);
        switch (irpStack.MinorFunction) {
        case IRP_MN_START_DEVICE:
        {
            //
            // The device is starting.
            // We cannot touch the device (send it any non pnp irps) until a
            // start device has been passed down to the lower drivers.
            //

            nt::Event startCompleted{NotificationEvent};
            IoCopyCurrentIrpStackLocationToNext(&irp);
            IoSetCompletionRoutine(&irp,
                FilterStartCompletionRoutine,
                &startCompleted,
                TRUE,
                TRUE,
                TRUE);
            status = IoCallDriver(m_lowerDevice, &irp);

            //
            // Wait for lower drivers to be done with the Irp. Important thing to
            // note here is when you allocate memory for an event in the stack  
            // you must do a KernelMode wait instead of UserMode to prevent 
            // the stack from getting paged out.
            //
            if (status == STATUS_PENDING) {
                startCompleted.Wait();
                status = irp.IoStatus.Status;
            }

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
        case IRP_MN_REMOVE_DEVICE:

            //
            // Wait for all outstanding requests to complete
            //
            removeLockGuard.release();
            m_removeLock.ReleaseAndWait(&irp);

            status = ForwardAndForgetNoLock(irp);

            IoDetachDevice(m_lowerDevice);
            this->~FilterDeviceExtension();
            IoDeleteDevice(irpStack.DeviceObject);

            return status;

        case IRP_MN_QUERY_STOP_DEVICE:
            status = STATUS_SUCCESS;
            break;

        case IRP_MN_CANCEL_STOP_DEVICE:

            status = STATUS_SUCCESS; // We must not fail this IRP.
            break;

        case IRP_MN_STOP_DEVICE:
            status = STATUS_SUCCESS;
            break;

        case IRP_MN_QUERY_REMOVE_DEVICE:

            status = STATUS_SUCCESS;
            break;

        case IRP_MN_SURPRISE_REMOVAL:

            status = STATUS_SUCCESS;
            break;

        case IRP_MN_CANCEL_REMOVE_DEVICE:

            status = STATUS_SUCCESS; // We must not fail this IRP.
            break;

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

            IoCopyCurrentIrpStackLocationToNext(&irp);

            IoSetCompletionRoutine(
                &irp,
                FilterDeviceUsageNotificationCompletionRoutine,
                NULL,
                TRUE,
                TRUE,
                TRUE
            );

            return IoCallDriver(m_lowerDevice, &irp);

        default:
            //
            // If you don't handle any IRP you must leave the
            // status as is.
            //
            status = irp.IoStatus.Status;

            break;
        }

        //
        // Pass the IRP down and forget it.
        //
        irp.IoStatus.Status = status;
        return ForwardAndForgetNoLock(irp);
    }


    NTSTATUS ForwardAndForget(IRP& irp) {
        const auto status = m_removeLock.Acquire(&irp);
        if (!NT_SUCCESS(status)) {
            return nt::Complete(irp, status);
        }
        const auto removeLockGuard = kcpp::scope_guard{ [this, &irp] {m_removeLock.Release(&irp); } };

        return ForwardAndForgetNoLock(irp);
    }

    void OnDeviceUsageNotificationComplete(DEVICE_OBJECT& deviceObject,
        PIRP irp) {
        const auto removeLockScopeGuard = [this, irp] { m_removeLock.Release(irp); };

        // On the way up, pagable might become clear.
        // Mimic the driver below us.
        if (!(m_lowerDevice->Flags & DO_POWER_PAGABLE)) {

            deviceObject.Flags &= ~DO_POWER_PAGABLE;
        }
    }

    FilterDeviceExtension(FilterDeviceExtension&&) = delete;

    void* operator new(size_t, void* p) { return p; }
private:
    nt::RemoveLock m_removeLock{ 'tlFB', 1, 100 };
    PDEVICE_OBJECT  m_lowerDevice{};

    NTSTATUS ForwardAndForgetNoLock(IRP& irp) {
        IoSkipCurrentIrpStackLocation(&irp);
        return IoCallDriver(m_lowerDevice, &irp);
    }
};

NTSTATUS FilterDeviceUsageNotificationCompletionRoutine(PDEVICE_OBJECT deviceObject,
    PIRP irp,
    [[maybe_unused]] PVOID context) {
    if (irp->PendingReturned) {
        IoMarkIrpPending(irp);
    }

    auto deviceExtension = static_cast<FilterDeviceExtension*>(deviceObject->DeviceExtension);
    deviceExtension->OnDeviceUsageNotificationComplete(*deviceObject, irp);

    return STATUS_CONTINUE_COMPLETION;
}

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

VOID
    Unload(
        [[maybe_unused]] __in PDRIVER_OBJECT DriverObject
    )
    /*++

    Routine Description:

        Free all the allocated resources in DriverEntry, etc.

    Arguments:

        DriverObject - pointer to a driver object.

    Return Value:

        VOID.

    --*/
{
    PAGED_CODE();

    //
    // The device object(s) should be NULL now
    // (since we unload, all the devices objects associated with this
    // driver must be deleted.
    //
    //if (DriverObject->DeviceObject != NULL) {
    //    ASSERTMSG("DeviceObject is not deleted ", FALSE);
    //}

    //
    // We should not be unloaded until all the devices we control
    // have been removed from our queue.
    //

    return;
}

alignas(Driver) char driverMemory[sizeof(Driver)];

}

Driver& g_driver = *reinterpret_cast<Driver*>(driverMemory);

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT driverObject,
    [[maybe_unused]] PUNICODE_STRING regPath) {

    for (auto& majorFunction : driverObject->MajorFunction) {
        majorFunction = DefaultDispatch;
    }

    driverObject->MajorFunction[IRP_MJ_PNP] = DispatchPnp;
    driverObject->DriverExtension->AddDevice = AddDevice;
    driverObject->DriverUnload = Unload;

    return STATUS_SUCCESS;
}
