#include "pch.h"

namespace {
    typedef struct _DEVICE_EXTENSION
    {
        //
        // The top of the stack before this filter was added.
        //

        PDEVICE_OBJECT  NextLowerDriver;

        //
        // Removelock to track IRPs so that device can be removed and
        // the driver can be unloaded safely.
        //
        IO_REMOVE_LOCK RemoveLock;

    } DEVICE_EXTENSION, * PDEVICE_EXTENSION;

    NTSTATUS
        FilterStartCompletionRoutine(
            PDEVICE_OBJECT   DeviceObject,
            PIRP             Irp,
            PVOID            Context
        )
        /*++
        Routine Description:
            A completion routine for use when calling the lower device objects to
            which our filter deviceobject is attached.

        Arguments:

            DeviceObject - Pointer to deviceobject
            Irp          - Pointer to a PnP Irp.
            Context      - NULL
        Return Value:

            NT Status is returned.

        --*/

    {
        PKEVENT             event = (PKEVENT)Context;

        UNREFERENCED_PARAMETER(DeviceObject);

        //
        // If the lower driver didn't return STATUS_PENDING, we don't need to 
        // set the event because we won't be waiting on it. 
        // This optimization avoids grabbing the dispatcher lock, and improves perf.
        //
        if (Irp->PendingReturned == TRUE) {
            KeSetEvent(event, IO_NO_INCREMENT, FALSE);
        }

        //
        // The dispatch routine will have to call IoCompleteRequest
        //

        return STATUS_MORE_PROCESSING_REQUIRED;

    }

    NTSTATUS
        FilterDeviceUsageNotificationCompletionRoutine(
            PDEVICE_OBJECT   DeviceObject,
            PIRP             Irp,
            PVOID            Context
        )
        /*++
        Routine Description:
            A completion routine for use when calling the lower device objects to
            which our filter deviceobject is attached.

        Arguments:

            DeviceObject - Pointer to deviceobject
            Irp          - Pointer to a PnP Irp.
            Context      - NULL
        Return Value:

            NT Status is returned.

        --*/

    {
        PDEVICE_EXTENSION       deviceExtension;

        UNREFERENCED_PARAMETER(Context);

        deviceExtension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;


        if (Irp->PendingReturned) {

            IoMarkIrpPending(Irp);
        }

        //
        // On the way up, pagable might become clear. Mimic the driver below us.
        //
        if (!(deviceExtension->NextLowerDriver->Flags & DO_POWER_PAGABLE)) {

            DeviceObject->Flags &= ~DO_POWER_PAGABLE;
        }

        IoReleaseRemoveLock(&deviceExtension->RemoveLock, Irp);

        return STATUS_CONTINUE_COMPLETION;

    }

    NTSTATUS DefaultDispatch(PDEVICE_OBJECT deviceObject, PIRP irp) {
        PDEVICE_EXTENSION           deviceExtension;
        NTSTATUS    status;

        deviceExtension = (PDEVICE_EXTENSION)deviceObject->DeviceExtension;
        status = IoAcquireRemoveLock(&deviceExtension->RemoveLock, irp);
        if (!NT_SUCCESS(status)) {
            return nt::Complete(*irp, status);
        }

        IoSkipCurrentIrpStackLocation(irp);
        status = IoCallDriver(deviceExtension->NextLowerDriver, irp);
        IoReleaseRemoveLock(&deviceExtension->RemoveLock, irp);
        return status;
    }

    NTSTATUS DispatchPnp(
            PDEVICE_OBJECT DeviceObject,
            PIRP Irp)
    {
        PDEVICE_EXTENSION           deviceExtension;
        PIO_STACK_LOCATION         irpStack;
        NTSTATUS                            status;
        KEVENT                               event;

        PAGED_CODE();

        deviceExtension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;
        irpStack = IoGetCurrentIrpStackLocation(Irp);

        status = IoAcquireRemoveLock(&deviceExtension->RemoveLock, Irp);
        if (!NT_SUCCESS(status)) {
            return nt::Complete(*Irp, status);
        }

        switch (irpStack->MinorFunction) {
        case IRP_MN_START_DEVICE:

            //
            // The device is starting.
            // We cannot touch the device (send it any non pnp irps) until a
            // start device has been passed down to the lower drivers.
            //
            KeInitializeEvent(&event, NotificationEvent, FALSE);
            IoCopyCurrentIrpStackLocationToNext(Irp);
            IoSetCompletionRoutine(Irp,
                FilterStartCompletionRoutine,
                &event,
                TRUE,
                TRUE,
                TRUE);

            status = IoCallDriver(deviceExtension->NextLowerDriver, Irp);

            //
            // Wait for lower drivers to be done with the Irp. Important thing to
            // note here is when you allocate memory for an event in the stack  
            // you must do a KernelMode wait instead of UserMode to prevent 
            // the stack from getting paged out.
            //
            if (status == STATUS_PENDING) {

                KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);
                status = Irp->IoStatus.Status;
            }

            if (NT_SUCCESS(status)) {

                //
                // On the way up inherit FILE_REMOVABLE_MEDIA during Start.
                // This characteristic is available only after the driver stack is started!.
                //
                if (deviceExtension->NextLowerDriver->Characteristics & FILE_REMOVABLE_MEDIA) {

                    DeviceObject->Characteristics |= FILE_REMOVABLE_MEDIA;
                }
            }

            Irp->IoStatus.Status = status;
            IoCompleteRequest(Irp, IO_NO_INCREMENT);
            IoReleaseRemoveLock(&deviceExtension->RemoveLock, Irp);
            return status;

        case IRP_MN_REMOVE_DEVICE:

            //
            // Wait for all outstanding requests to complete
            //
            IoReleaseRemoveLockAndWait(&deviceExtension->RemoveLock, Irp);

            IoSkipCurrentIrpStackLocation(Irp);

            status = IoCallDriver(deviceExtension->NextLowerDriver, Irp);

            IoDetachDevice(deviceExtension->NextLowerDriver);
            IoDeleteDevice(DeviceObject);
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
            if ((DeviceObject->AttachedDevice == NULL) ||
                (DeviceObject->AttachedDevice->Flags & DO_POWER_PAGABLE)) {

                DeviceObject->Flags |= DO_POWER_PAGABLE;
            }

            IoCopyCurrentIrpStackLocationToNext(Irp);

            IoSetCompletionRoutine(
                Irp,
                FilterDeviceUsageNotificationCompletionRoutine,
                NULL,
                TRUE,
                TRUE,
                TRUE
            );

            return IoCallDriver(deviceExtension->NextLowerDriver, Irp);

        default:
            //
            // If you don't handle any IRP you must leave the
            // status as is.
            //
            status = Irp->IoStatus.Status;

            break;
        }

        //
        // Pass the IRP down and forget it.
        //
        Irp->IoStatus.Status = status;
        IoSkipCurrentIrpStackLocation(Irp);
        status = IoCallDriver(deviceExtension->NextLowerDriver, Irp);
        IoReleaseRemoveLock(&deviceExtension->RemoveLock, Irp);
        return status;
    }

    NTSTATUS
        AddDevice(
            __in PDRIVER_OBJECT DriverObject,
            __in PDEVICE_OBJECT PhysicalDeviceObject
        )
        /*++

        Routine Description:

            The Plug & Play subsystem is handing us a brand new PDO, for which we
            (by means of INF registration) have been asked to provide a driver.

            We need to determine if we need to be in the driver stack for the device.
            Create a function device object to attach to the stack
            Initialize that device object
            Return status success.

            Remember: We can NOT actually send ANY non pnp IRPS to the given driver
            stack, UNTIL we have received an IRP_MN_START_DEVICE.

        Arguments:

            DeviceObject - pointer to a device object.

            PhysicalDeviceObject -  pointer to a device object created by the
                                    underlying bus driver.

        Return Value:

            NT status code.

        --*/
    {
        NTSTATUS                status = STATUS_SUCCESS;
        PDEVICE_OBJECT          deviceObject = NULL;
        PDEVICE_EXTENSION       deviceExtension;
        ULONG                   deviceType = FILE_DEVICE_UNKNOWN;

        PAGED_CODE();


        deviceObject = IoGetAttachedDeviceReference(PhysicalDeviceObject);
        deviceType = deviceObject->DeviceType;
        ObDereferenceObject(deviceObject);

        //
        // Create a filter device object.
        //

        status = IoCreateDevice(DriverObject,
            sizeof(DEVICE_EXTENSION),
            NULL,  // No Name
            deviceType,
            FILE_DEVICE_SECURE_OPEN,
            FALSE,
            &deviceObject);


        if (!NT_SUCCESS(status)) {
            //
            // Returning failure here prevents the entire stack from functioning,
            // but most likely the rest of the stack will not be able to create
            // device objects either, so it is still OK.
            //
            return status;
        }

        deviceExtension = (PDEVICE_EXTENSION)deviceObject->DeviceExtension;

        deviceExtension->NextLowerDriver = IoAttachDeviceToDeviceStack(
            deviceObject,
            PhysicalDeviceObject);
        //
        // Failure for attachment is an indication of a broken plug & play system.
        //

        if (NULL == deviceExtension->NextLowerDriver) {

            IoDeleteDevice(deviceObject);
            return STATUS_UNSUCCESSFUL;
        }

        deviceObject->Flags |= deviceExtension->NextLowerDriver->Flags &
            (DO_BUFFERED_IO | DO_DIRECT_IO |
                DO_POWER_PAGABLE);


        deviceObject->DeviceType = deviceExtension->NextLowerDriver->DeviceType;

        deviceObject->Characteristics =
            deviceExtension->NextLowerDriver->Characteristics;

        //
        // Let us use remove lock to keep count of IRPs so that we don't 
        // deteach and delete our deviceobject until all pending I/Os in our
        // devstack are completed. Remlock is required to protect us from
        // various race conditions where our driver can get unloaded while we
        // are still running dispatch or completion code.
        //

        IoInitializeRemoveLock(&deviceExtension->RemoveLock,
            'tlFB',
            1, // MaxLockedMinutes 
            100); // HighWatermark, this parameter is 
        // used only on checked build. Specifies 
        // the maximum number of outstanding 
        // acquisitions allowed on the lock


    //
    // Set the initial state of the Filter DO
    //

        deviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

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

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT driverObject, [[maybe_unused]] PUNICODE_STRING regPath) {

    for (auto& majorFunction : driverObject->MajorFunction) {
        majorFunction = DefaultDispatch;
    }

    driverObject->MajorFunction[IRP_MJ_PNP] = DispatchPnp;
    driverObject->DriverExtension->AddDevice = AddDevice;
    driverObject->DriverUnload = Unload;

    return STATUS_SUCCESS;
}
