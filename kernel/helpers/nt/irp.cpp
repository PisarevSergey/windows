#include "irp.h"

namespace nt {

    NTSTATUS SendWithCompletionRoutine(DEVICE_OBJECT& target, IRP& irp, IO_COMPLETION_ROUTINE& completionRoutine, void* completionContext) {

        IoCopyCurrentIrpStackLocationToNext(&irp);
        IoSetCompletionRoutine(&irp,
            completionRoutine,
            completionContext,
            TRUE,
            TRUE,
            TRUE);

        return IoCallDriver(&target, &irp);

    }

    NTSTATUS Complete(IRP& irp, NTSTATUS status, CCHAR priorityBoost) {
        irp.IoStatus.Status = status;
        IoCompleteRequest(&irp, priorityBoost);
        return status;
    }

}

