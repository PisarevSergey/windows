#include "pch.h"

#include <kcpp/scope_guard.h>

TRACELOGGING_DEFINE_PROVIDER(g_tracer, "NetDataMonLoggingProviderKM", (0xa087757e, 0xa1d9, 0x4261, 0x97, 0x1c, 0xe4, 0x17, 0x3c, 0x4b, 0x7d, 0x1e));

namespace
{
    NTSTATUS Unload([[maybe_unused]] FLT_FILTER_UNLOAD_FLAGS flags)
    {
        TraceLoggingWrite(g_tracer, "unloading");
        g_driver.~Driver();

        return STATUS_SUCCESS;
    }

    NTSTATUS OnConnect([[maybe_unused]] PFLT_PORT ClientPort,
        [[maybe_unused]] PVOID ServerPortCookie,
        [[maybe_unused]] PVOID ConnectionContext,
        [[maybe_unused]] ULONG SizeOfContext,
        [[maybe_unused]] PVOID* ConnectionPortCookie)
    {
        return STATUS_SUCCESS;
    }

    void OnDisconnect([[maybe_unused]] PVOID ConnectionCookie)
    {}

    NTSTATUS OnMessageNotify(
        [[maybe_unused]] PVOID PortCookie,
        [[maybe_unused]] PVOID InputBuffer OPTIONAL,
        [[maybe_unused]] ULONG InputBufferLength,
        [[maybe_unused]] PVOID OutputBuffer OPTIONAL,
        [[maybe_unused]] ULONG OutputBufferLength,
        [[maybe_unused]] PULONG ReturnOutputBufferLength
    )
    {
        return STATUS_SUCCESS;
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
        TraceLoggingWrite(g_tracer, "driver init failed", TraceLoggingLevel(WINEVENT_LEVEL_ERROR), TraceLoggingNTStatus(status, "error"));
        return status;
    }

    TraceLoggingWrite(g_tracer, "driver started", TraceLoggingLevel(WINEVENT_LEVEL_INFO));
    driverGuard.release();
    return STATUS_SUCCESS;
}

NTSTATUS Driver::Init(DRIVER_OBJECT& driverObject)
{
    NTSTATUS status{};
    m_minifilter = flt::CreateMinifilter(status, driverObject, Unload);
    if (!NT_SUCCESS(status))
    {
        TraceLoggingWrite(g_tracer,
            "CreateMinifilter failed",
            TraceLoggingLevel(WINEVENT_LEVEL_ERROR),
            TraceLoggingNTStatus(status));

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
        TraceLoggingWrite(g_tracer, "failed to create server communication port", TraceLoggingLevel(WINEVENT_LEVEL_ERROR), TraceLoggingNTStatus(status));
        return status;
    }

    m_wfpDevice = nt::CreateDevice(status, driverObject);
    if (!NT_SUCCESS(status))
    {
        TraceLoggingWrite(g_tracer,
            "failed to create WFP device",
            TraceLoggingLevel(WINEVENT_LEVEL_ERROR),
            TraceLoggingNTStatus(status));

        return status;
    }

    status = m_callouts.Init(*m_wfpDevice);
    if (!NT_SUCCESS(status))
    {
        TraceLoggingWrite(g_tracer,
            "callout initialization failed",
            TraceLoggingLevel(WINEVENT_LEVEL_ERROR),
            TraceLoggingNTStatus(status));

        return status;
    }

    TraceLoggingWrite(g_tracer, "driver init success", TraceLoggingLevel(WINEVENT_LEVEL_INFO));
    return STATUS_SUCCESS;
}
