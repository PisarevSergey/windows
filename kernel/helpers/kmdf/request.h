#pragma once

#include <ntddk.h>

#pragma warning(push)
#pragma warning(disable : 4471)
#include <wdf.h>
#pragma warning(pop)

namespace kmdf {
    WDF_REQUEST_SEND_OPTIONS CreateRequestSendOptions(ULONG flags);

    WDF_REQUEST_PARAMETERS GetRequestParameters(WDFREQUEST request);

    [[nodiscard]]
    NTSTATUS ForwardAndForget(WDFREQUEST request, WDFIOTARGET target);

    [[nodiscard]]
    NTSTATUS ForwardWithCompletion(WDFREQUEST request, WDFIOTARGET target, PFN_WDF_REQUEST_COMPLETION_ROUTINE completion, WDFCONTEXT completionContext = nullptr);
}