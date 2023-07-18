#include "pch.h"

#include <kcpp/scope_guard.h>

namespace
{
    wchar_t sessionName[]{L"NetDataMon session"};
    wchar_t sessionDescription[]{ L"NetDataMon session description" };

    [[nodiscard]]
    NTSTATUS AddSublayer(HANDLE engine, const FWPM_SUBLAYER sublayer)
    {
        TraceLoggingWrite(g_tracer,
            "adding sublayer",
            TraceLoggingLevel(WINEVENT_LEVEL_INFO),
            TraceLoggingValue(sublayer.subLayerKey),
            TraceLoggingValue(sublayer.displayData.name),
            TraceLoggingValue(sublayer.displayData.description));

        const auto status = FwpmSubLayerAdd(engine, &sublayer, nullptr);
        if (STATUS_SUCCESS != status)
        {
            TraceLoggingWrite(g_tracer,
                "FwpmSubLayerAdd failed",
                TraceLoggingLevel(WINEVENT_LEVEL_ERROR),
                TraceLoggingNTStatus(status));

            return status;
        }

        TraceLoggingWrite(g_tracer, "FwpmSubLayerAdd success", TraceLoggingLevel(WINEVENT_LEVEL_INFO));

        return STATUS_SUCCESS;
    }

    [[nodiscard]]
    NTSTATUS AddCallout(HANDLE engine, const FWPM_CALLOUT& callout)
    {
        TraceLoggingWrite(g_tracer,
            "adding callout",
            TraceLoggingLevel(WINEVENT_LEVEL_INFO),
            TraceLoggingValue(callout.calloutKey),
            TraceLoggingValue(callout.displayData.name),
            TraceLoggingValue(callout.displayData.description),
            TraceLoggingValue(callout.applicableLayer));

        UINT32 calloutId{};
        const auto status = FwpmCalloutAdd(engine, &callout, nullptr, &calloutId);
        if (STATUS_SUCCESS != status)
        {
            TraceLoggingWrite(g_tracer,
                "FwpmCalloutAdd failed",
                TraceLoggingLevel(WINEVENT_LEVEL_ERROR),
                TraceLoggingNTStatus(status));

            return status;
        }

        TraceLoggingWrite(g_tracer, "FwpmCalloutAdd success", TraceLoggingLevel(WINEVENT_LEVEL_INFO));

        return STATUS_SUCCESS;
    }

    [[nodiscard]]
    NTSTATUS AddFilter(HANDLE engine, const FWPM_FILTER& filter)
    {
        TraceLoggingWrite(g_tracer,
            "adding filter",
            TraceLoggingLevel(WINEVENT_LEVEL_INFO),
            TraceLoggingValue(filter.filterKey),
            TraceLoggingValue(filter.displayData.name),
            TraceLoggingValue(filter.displayData.description));

        UINT64 filterId{};
        const auto status = FwpmFilterAdd(engine, &filter, nullptr, &filterId);
        if (STATUS_SUCCESS != status)
        {
            TraceLoggingWrite(g_tracer,
                "FwpmFilterAdd failed",
                TraceLoggingLevel(WINEVENT_LEVEL_ERROR),
                TraceLoggingNTStatus(status));

            return status;
        }

        TraceLoggingWrite(g_tracer,
            "FwpmFilterAdd success",
            TraceLoggingLevel(WINEVENT_LEVEL_INFO),
            TraceLoggingValue(filterId));

        return STATUS_SUCCESS;
    }

    [[nodiscard]]
    NTSTATUS AddBfeObjects(HANDLE engine)
    {
        for (const auto& currentWfpObjects : wfpObjects)
        {
            const auto& objectsToRegister = currentWfpObjects.management;

            auto status = AddSublayer(engine, objectsToRegister.m_sublayer);
            if (STATUS_SUCCESS != status)
            {
                return status;
            }

            status = AddCallout(engine, objectsToRegister.m_callout);
            if (STATUS_SUCCESS != status)
            {
                return status;
            }

            status = AddFilter(engine, objectsToRegister.m_filter);
            if (STATUS_SUCCESS != status)
            {
                return status;
            }
        }

        return STATUS_SUCCESS;
    }

    [[nodiscard]]
    NTSTATUS AddBfeObjectsTransacted(HANDLE engine)
    {
        auto status = FwpmTransactionBegin(engine, 0);
        if (STATUS_SUCCESS != status)
        {
            TraceLoggingWrite(g_tracer,
                "FwpmTransactionBegin failed",
                TraceLoggingLevel(WINEVENT_LEVEL_ERROR),
                TraceLoggingNTStatus(status));

            return status;
        }

        kcpp::scope_guard transactionAborter{[engine] {FwpmTransactionAbort(engine); } };

        status = AddBfeObjects(engine);
        if (STATUS_SUCCESS != status)
        {
            return status;
        }

        status = FwpmTransactionCommit(engine);
        if (STATUS_SUCCESS != status)
        {
            TraceLoggingWrite(g_tracer,
                "FwpmTransactionCommit failed",
                TraceLoggingLevel(WINEVENT_LEVEL_ERROR),
                TraceLoggingNTStatus(status));

            return status;
        }

        transactionAborter.release();

        TraceLoggingWrite(g_tracer, "add objects to BFE success", TraceLoggingLevel(WINEVENT_LEVEL_INFO));

        return STATUS_SUCCESS;
    }
}

AutoBfe CreateBfeObjects(NTSTATUS& status)
{
    constexpr FWPM_SESSION sessionParams
    {
        .sessionKey{ 0xf64af6cd, 0xc098, 0x4cb0, { 0x81, 0xd7, 0xdb, 0x7f, 0xb9, 0x9e, 0x37, 0x4c }},
        .displayData{.name{sessionName}, .description{sessionDescription}},
        .flags{FWPM_SESSION_FLAG_DYNAMIC},
        .kernelMode{TRUE}
    };

    AutoBfe engine;
    status = FwpmEngineOpen(nullptr, RPC_C_AUTHN_WINNT, nullptr, &sessionParams, static_cast<HANDLE*>(engine));
    if (STATUS_SUCCESS != status)
    {
        static_cast<void>(engine.release());

        TraceLoggingWrite(g_tracer, "FwpmEngineOpen failed", TraceLoggingLevel(WINEVENT_LEVEL_ERROR), TraceLoggingNTStatus(status));

        return AutoBfe{};
    }

    status = AddBfeObjectsTransacted(engine.get());
    if (STATUS_SUCCESS != status)
    {
        return AutoBfe{};
    }

    return engine;
}
