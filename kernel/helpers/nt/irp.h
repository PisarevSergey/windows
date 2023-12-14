#pragma once

#include <wdm.h>

namespace nt {
    NTSTATUS Complete(IRP& irp, NTSTATUS status, CCHAR priorityBoost = IO_NO_INCREMENT);
}