#include "pch.h"

#include <kcpp/scope_guard.h>

namespace {
    wchar_t sessionName[]{ L"NetDataMon session" };
    wchar_t sessionDescription[]{ L"NetDataMon session description" };

    [[nodiscard]]
    NTSTATUS AddSublayer(HANDLE engine, const FWPM_SUBLAYER sublayer) {
        TraceInfo("adding sublayer",
            TraceLoggingValue(sublayer.subLayerKey),
            TraceLoggingValue(sublayer.displayData.name),
            TraceLoggingValue(sublayer.displayData.description));

        const auto status = FwpmSubLayerAdd(engine, &sublayer, nullptr);
        if (STATUS_SUCCESS != status) {
            TraceError("FwpmSubLayerAdd failed",
                TraceLoggingNTStatus(status));

            return status;
        }

        TraceInfo("FwpmSubLayerAdd success");

        return STATUS_SUCCESS;
    }

    [[nodiscard]]
    NTSTATUS AddCallout(HANDLE engine, const FWPM_CALLOUT& callout) {
        TraceInfo("adding callout",
            TraceLoggingValue(callout.calloutKey),
            TraceLoggingValue(callout.displayData.name),
            TraceLoggingValue(callout.displayData.description),
            TraceLoggingValue(callout.applicableLayer));

        UINT32 calloutId{};
        const auto status = FwpmCalloutAdd(engine, &callout, nullptr, &calloutId);
        if (STATUS_SUCCESS != status) {
            TraceError("FwpmCalloutAdd failed", TraceLoggingNTStatus(status));

            return status;
        }

        TraceInfo("FwpmCalloutAdd success");

        return STATUS_SUCCESS;
    }

    [[nodiscard]]
    NTSTATUS AddFilter(HANDLE engine, const FWPM_FILTER& filter) {
        TraceInfo("adding filter",
            TraceLoggingValue(filter.filterKey),
            TraceLoggingValue(filter.displayData.name),
            TraceLoggingValue(filter.displayData.description));

        UINT64 filterId{};
        const auto status = FwpmFilterAdd(engine, &filter, nullptr, &filterId);
        if (STATUS_SUCCESS != status) {
            TraceError("FwpmFilterAdd failed", TraceLoggingNTStatus(status));

            return status;
        }

        TraceInfo("FwpmFilterAdd success", TraceLoggingValue(filterId));

        return STATUS_SUCCESS;
    }

    [[nodiscard]]
    NTSTATUS AddBfeObjects(HANDLE engine) {
        for (const auto& currentWfpObjects : wfpObjects) {
            const auto& objectsToRegister = currentWfpObjects.management;

            auto status = AddSublayer(engine, objectsToRegister.m_sublayer);
            if (STATUS_SUCCESS != status) {
                return status;
            }

            status = AddCallout(engine, objectsToRegister.m_callout);
            if (STATUS_SUCCESS != status) {
                return status;
            }

            status = AddFilter(engine, objectsToRegister.m_filter);
            if (STATUS_SUCCESS != status) {
                return status;
            }
        }

        return STATUS_SUCCESS;
    }

    [[nodiscard]]
    NTSTATUS AddBfeObjectsTransacted(HANDLE engine) {
        auto status = FwpmTransactionBegin(engine, 0);
        if (STATUS_SUCCESS != status) {
            TraceError("FwpmTransactionBegin failed", TraceLoggingNTStatus(status));

            return status;
        }

        kcpp::scope_guard transactionAborter{ [engine] { FwpmTransactionAbort(engine); } };

        status = AddBfeObjects(engine);
        if (STATUS_SUCCESS != status) {
            return status;
        }

        status = FwpmTransactionCommit(engine);
        if (STATUS_SUCCESS != status) {
            TraceError("FwpmTransactionCommit failed", TraceLoggingNTStatus(status));

            return status;
        }

        transactionAborter.release();

        TraceInfo("add objects to BFE success");

        return STATUS_SUCCESS;
    }
}

void BfeCloser::operator()(HANDLE h) const {
    const auto status = FwpmEngineClose(h);
    if (STATUS_SUCCESS != status) {
        TraceCritical("FwpmEngineClose failed", TraceLoggingNTStatus(status));
    }

    NT_ASSERT(STATUS_SUCCESS == status);
}

AutoBfe CreateBfeObjects(NTSTATUS& status) {
    constexpr FWPM_SESSION sessionParams{
        .sessionKey{ 0xf64af6cd, 0xc098, 0x4cb0, { 0x81, 0xd7, 0xdb, 0x7f, 0xb9, 0x9e, 0x37, 0x4c }},
        .displayData{.name{sessionName}, .description{sessionDescription}},
        .flags{FWPM_SESSION_FLAG_DYNAMIC},
        .kernelMode{TRUE}
    };

    AutoBfe engine;
    status = FwpmEngineOpen(nullptr, RPC_C_AUTHN_WINNT, nullptr, &sessionParams, static_cast<HANDLE*>(engine));
    if (STATUS_SUCCESS != status) {
        static_cast<void>(engine.release());

        TraceError("FwpmEngineOpen failed", TraceLoggingNTStatus(status));

        return AutoBfe{};
    }

    status = AddBfeObjectsTransacted(engine.get());
    if (STATUS_SUCCESS != status) {
        return AutoBfe{};
    }

    return engine;
}

void BfeChangeHandleReleaser::operator()(HANDLE changeHandle) const {
    const auto status = FwpmBfeStateUnsubscribeChanges(changeHandle);
    if (STATUS_SUCCESS == status) {
        TraceInfo("FwpmBfeStateUnsubscribeChanges success");
    }
    else {
        TraceCritical("FwpmBfeStateUnsubscribeChanges failed", TraceLoggingNTStatus(status));
    }

    NT_ASSERT(STATUS_SUCCESS == status);
}

NTSTATUS BfeStateSubscriber::Init(void* deviceObject, FWPM_SERVICE_STATE_CHANGE_CALLBACK0 callback, void* context) {
    HANDLE changeHandle{};
    const auto status = FwpmBfeStateSubscribeChanges(deviceObject, callback, context, &changeHandle);
    if (STATUS_SUCCESS != status) {
        TraceError("FwpmBfeStateSubscribeChanges failed", TraceLoggingNTStatus(status));
        return status;
    }

    m_changeHandle.reset(changeHandle);
    return STATUS_SUCCESS;
}
