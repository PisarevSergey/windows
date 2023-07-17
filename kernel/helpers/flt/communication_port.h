#pragma once

#include <fltKernel.h>

#include <kcpp/auto_ptr.h>

namespace flt
{
    struct ServerPortCloser final
    {
        void operator()(PFLT_PORT serverPort) const;
    };

    using AutoServerPort = kcpp::auto_ptr<PFLT_PORT, ServerPortCloser>;

    AutoServerPort CreateCommunicationPort(NTSTATUS& status,
        PFLT_FILTER filter,
        const wchar_t* portName,
        PFLT_CONNECT_NOTIFY connectNotifyCallback,
        PFLT_DISCONNECT_NOTIFY disconnectNotifyCallback,
        PFLT_MESSAGE_NOTIFY messageNotifyCallback,
        void* serverPortCookie = nullptr,
        LONG maxConnections = 1);
}
