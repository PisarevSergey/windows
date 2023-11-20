#include "request.h"

WDF_REQUEST_SEND_OPTIONS kmdf::CreateRequestSendOptions(ULONG flags)
{
    WDF_REQUEST_SEND_OPTIONS options;
    WDF_REQUEST_SEND_OPTIONS_INIT(&options, flags);
    return options;
}

NTSTATUS kmdf::ForwardAndForget(WDFREQUEST request, WDFIOTARGET target) {
    auto sendOptions = CreateRequestSendOptions(WDF_REQUEST_SEND_OPTION_SEND_AND_FORGET);
    return WdfRequestSend(request, target, &sendOptions) ? STATUS_SUCCESS : WdfRequestGetStatus(request);
}
