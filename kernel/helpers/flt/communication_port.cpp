#include "communication_port.h"

#include <trace_logging/trace.h>

namespace
{
    struct SecurityDescriptorReleaser final
    {
        void operator()(PSECURITY_DESCRIPTOR sd) const
        {
            FltFreeSecurityDescriptor(sd);
        }
    };

    using AutoSecurityDescriptor = kcpp::auto_ptr<PSECURITY_DESCRIPTOR, SecurityDescriptorReleaser>;

    AutoSecurityDescriptor CreateSecurityDescriptor(NTSTATUS& status, ACCESS_MASK desiredAccess)
    {
        PSECURITY_DESCRIPTOR sd{};
        status = FltBuildDefaultSecurityDescriptor(&sd, desiredAccess);
        if (!NT_SUCCESS(status))
        {
            TraceLoggingWrite(g_tracer, "FltBuildDefaultSecurityDescriptor failed", TraceLoggingLevel(WINEVENT_LEVEL_ERROR), TraceLoggingNTStatus(status));
            return AutoSecurityDescriptor{};
        }

        return AutoSecurityDescriptor{ sd };
    }
}

namespace flt
{
    void ServerPortCloser::operator()(PFLT_PORT serverPort) const
    {
        FltCloseCommunicationPort(serverPort);
    }

    AutoServerPort CreateCommunicationPort(NTSTATUS& status,
        PFLT_FILTER filter,
        const wchar_t* portName,
        PFLT_CONNECT_NOTIFY connectNotifyCallback,
        PFLT_DISCONNECT_NOTIFY disconnectNotifyCallback,
        PFLT_MESSAGE_NOTIFY messageNotifyCallback,
        void* serverPortCookie,
        LONG maxConnections)
    {
        auto securityDescriptor = CreateSecurityDescriptor(status, FLT_PORT_ALL_ACCESS);
        if (!NT_SUCCESS(status))
        {
            return AutoServerPort{};
        }

        UNICODE_STRING portNameUs{};
        RtlInitUnicodeString(&portNameUs, portName);

        OBJECT_ATTRIBUTES oa;
        InitializeObjectAttributes(&oa, &portNameUs, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, nullptr, securityDescriptor.get());

        PFLT_PORT serverPort{};
        status = FltCreateCommunicationPort(filter,
            &serverPort,
            &oa,
            serverPortCookie,
            connectNotifyCallback,
            disconnectNotifyCallback,
            messageNotifyCallback,
            maxConnections);
        if (!NT_SUCCESS(status))
        {
            TraceLoggingWrite(g_tracer, "FltCreateCommunicationPort failed", TraceLoggingLevel(WINEVENT_LEVEL_ERROR), TraceLoggingNTStatus(status));
            return AutoServerPort{};
        }

        return AutoServerPort{serverPort};
    }
}
