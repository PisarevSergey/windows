#include "driver.h"

#include <kcpp/scope_guard.h>

TRACELOGGING_DEFINE_PROVIDER(g_tracer, "NetDataMonLoggingProviderKM", (0xa087757e, 0xa1d9, 0x4261, 0x97, 0x1c, 0xe4, 0x17, 0x3c, 0x4b, 0x7d, 0x1e));

namespace
{
    NTSTATUS Unload([[maybe_unused]] FLT_FILTER_UNLOAD_FLAGS flags)
    {
        TraceInfo("unloading");
        g_driver.~Driver();

        return STATUS_SUCCESS;
    }

    NTSTATUS OnConnect(PFLT_PORT ClientPort,
        PVOID ServerPortCookie,
        [[maybe_unused]] PVOID ConnectionContext,
        [[maybe_unused]] ULONG SizeOfContext,
        PVOID* ConnectionPortCookie)
    {
        auto clientCommunicationPort = static_cast<ClientCommunicationPort*>(ServerPortCookie);
        *ConnectionPortCookie = clientCommunicationPort;

        return clientCommunicationPort->OnConnect(ClientPort);
    }

    void OnDisconnect(PVOID ConnectionCookie)
    {
        auto clientCommunicationPort = static_cast<ClientCommunicationPort*>(ConnectionCookie);
        clientCommunicationPort->OnDisconnect();
    }

    NTSTATUS OnMessageNotify(PVOID PortCookie,
        PVOID InputBuffer OPTIONAL,
        ULONG InputBufferLength,
        PVOID OutputBuffer OPTIONAL,
        ULONG OutputBufferLength,
        PULONG ReturnOutputBufferLength)
    {
        auto clientCommunicationPort = static_cast<ClientCommunicationPort*>(PortCookie);
        return clientCommunicationPort->OnMessageNotify(InputBuffer, InputBufferLength, OutputBuffer, OutputBufferLength, *ReturnOutputBufferLength);
    }

    void NTAPI BfeStateChangeCallback([[maybe_unused]] void* context, FWPM_SERVICE_STATE  newState)
    {
        auto bfeHandle = static_cast<AutoBfe*>(context);

        switch (newState)
        {
        case FWPM_SERVICE_RUNNING:
        {
            NTSTATUS status{};
            *bfeHandle = CreateBfeObjects(status);
            if (STATUS_SUCCESS == status)
            {
                TraceInfo("CreateBfeObjects success");
            }
            else
            {
                TraceError("CreateBfeObjects failed", TraceLoggingNTStatus(status));
            }

        }
            break;
        case FWPM_SERVICE_STOP_PENDING:
            TraceInfo("BFE stopping");
            bfeHandle->reset();
            break;
        }
    }

    alignas(Driver) char driverMemory[sizeof(Driver)];
}

Driver& g_driver = *reinterpret_cast<Driver*>(driverMemory);

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT driverObject, [[maybe_unused]] PUNICODE_STRING regPath)
{
    new(&g_driver) Driver;
    kcpp::scope_guard driverGuard{[] {g_driver.~Driver(); } };

    const auto status = g_driver.Init(*driverObject);
    if (!NT_SUCCESS(status))
    {
        TraceError("driver init failed", TraceLoggingNTStatus(status));
        return status;
    }

    TraceInfo("driver started");
    driverGuard.release();
    return STATUS_SUCCESS;
}

NTSTATUS Driver::Init(DRIVER_OBJECT& driverObject)
{
    NTSTATUS status{};
    m_minifilter = flt::CreateMinifilter(status, driverObject, Unload);
    if (!NT_SUCCESS(status))
    {
        TraceError("CreateMinifilter failed", TraceLoggingNTStatus(status));

        return status;
    }

    m_serverCommunicationPort = flt::CreateCommunicationPort(status,
        m_minifilter.get(),
        LR"(\NetDataMonCommunicationPort)",
        OnConnect,
        OnDisconnect,
        OnMessageNotify,
        &m_clientCommunicationPort);
    if (!NT_SUCCESS(status))
    {
        TraceError("failed to create server communication port", TraceLoggingNTStatus(status));
        return status;
    }

    m_wfpDevice = nt::CreateDevice(status, driverObject);
    if (!NT_SUCCESS(status))
    {
        TraceError("failed to create WFP device", TraceLoggingNTStatus(status));

        return status;
    }

    status = m_callouts.Init(*m_wfpDevice);
    if (STATUS_SUCCESS != status)
    {
        TraceError("callout initialization failed", TraceLoggingNTStatus(status));

        return status;
    }

    status = m_bfeStateSubscriber.Init(m_wfpDevice.get(), BfeStateChangeCallback, &m_bfe);
    if (STATUS_SUCCESS != status)
    {
        TraceError("failed to subscribe to BFE state change", TraceLoggingNTStatus(status));
        return status;
    }

    TraceInfo("driver init success");
    return STATUS_SUCCESS;
}

NTSTATUS ClientCommunicationPort::OnConnect(PFLT_PORT clientPort)
{
    TraceInfo("client port connect");

    NT_ASSERT(m_filter.get());

    m_port = clientPort;
    return STATUS_SUCCESS;
}

void ClientCommunicationPort::OnDisconnect()
{
    TraceInfo("client port disconnect");

    NT_ASSERT(m_filter.get());
    FltCloseClientPort(m_filter.get(), &m_port);
}

NTSTATUS ClientCommunicationPort::SendMessage(void* senderBuffer,
    ULONG senderBufferSize,
    PLARGE_INTEGER timeout,
    void* replyBuffer,
    PULONG replyBufferSize)
{
    return FltSendMessage(m_filter.get(), &m_port, senderBuffer, senderBufferSize, replyBuffer, replyBufferSize, timeout);
}

NTSTATUS ClientCommunicationPort::OnMessageNotify([[maybe_unused]] PVOID InputBuffer OPTIONAL,
    [[maybe_unused]] ULONG InputBufferLength,
    [[maybe_unused]]  PVOID OutputBuffer OPTIONAL,
    [[maybe_unused]] ULONG OutputBufferLength,
    ULONG& ReturnOutputBufferLength)
{
    ReturnOutputBufferLength = 0;
    return STATUS_NOT_IMPLEMENTED;
}
