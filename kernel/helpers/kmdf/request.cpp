#include "request.h"

namespace kmdf {
    WDF_REQUEST_SEND_OPTIONS CreateRequestSendOptions(ULONG flags)
    {
        WDF_REQUEST_SEND_OPTIONS options;
        WDF_REQUEST_SEND_OPTIONS_INIT(&options, flags);
        return options;
    }

    WDF_REQUEST_PARAMETERS GetRequestParameters(WDFREQUEST request)
    {
        WDF_REQUEST_PARAMETERS requestParameters;
        WDF_REQUEST_PARAMETERS_INIT(&requestParameters);

        WdfRequestGetParameters(request, &requestParameters);

        return requestParameters;
    }

    NTSTATUS ForwardAndForget(WDFREQUEST request, WDFIOTARGET target) {
        auto sendOptions = CreateRequestSendOptions(WDF_REQUEST_SEND_OPTION_SEND_AND_FORGET);
        return WdfRequestSend(request, target, &sendOptions) ? STATUS_SUCCESS : WdfRequestGetStatus(request);
    }

    NTSTATUS ForwardWithCompletion(WDFREQUEST request, WDFIOTARGET target, PFN_WDF_REQUEST_COMPLETION_ROUTINE completion, WDFCONTEXT completionContext)
    {
        NT_ASSERT(completion != nullptr);

        WdfRequestFormatRequestUsingCurrentType(request);
        WdfRequestSetCompletionRoutine(request, completion, completionContext);

        return WdfRequestSend(request, target, WDF_NO_SEND_OPTIONS) ? STATUS_SUCCESS : WdfRequestGetStatus(request);
    }
}

