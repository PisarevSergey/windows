#pragma once

#include <ntddk.h>

#pragma warning(push)
#pragma warning(disable : 4471)
#include <wdf.h>
#pragma warning(pop)

namespace kmdf {
    WDF_REQUEST_SEND_OPTIONS CreateRequestSendOptions(ULONG flags);

    NTSTATUS ForwardAndForget(WDFREQUEST request, WDFIOTARGET target);
}