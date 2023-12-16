#pragma once

#include <wdm.h>

namespace nt {

    NTSTATUS SendWithCompletionRoutine(DEVICE_OBJECT& target, IRP& irp, IO_COMPLETION_ROUTINE& completionRoutine, void* completionContext = nullptr);

    NTSTATUS Complete(IRP& irp, NTSTATUS status, CCHAR priorityBoost = IO_NO_INCREMENT);
}