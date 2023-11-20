#pragma once

#include <ntddk.h>

#pragma warning(push)
#pragma warning(disable : 4471)
#include <wdf.h>
#pragma warning(pop)

namespace kmdf {
    WDF_IO_QUEUE_CONFIG CreateQueueConfig(bool defaultQueue = true, WDF_IO_QUEUE_DISPATCH_TYPE dispathType = WdfIoQueueDispatchParallel);
}
