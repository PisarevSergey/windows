#include "queue.h"

WDF_IO_QUEUE_CONFIG kmdf::CreateQueueConfig(bool defaultQueue, WDF_IO_QUEUE_DISPATCH_TYPE dispathType)
{
    WDF_IO_QUEUE_CONFIG queueConfig;
    if (defaultQueue)
    {
        WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&queueConfig, dispathType);
    }
    else
    {
        WDF_IO_QUEUE_CONFIG_INIT(&queueConfig, dispathType);
    }

    return queueConfig;
}
