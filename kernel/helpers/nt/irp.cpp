#include "irp.h"

namespace nt {

    NTSTATUS Complete(IRP& irp, NTSTATUS status, CCHAR priorityBoost)
    {
        irp.IoStatus.Status = status;
        IoCompleteRequest(&irp, priorityBoost);
        return status;
    }

}

