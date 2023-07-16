#include "pch.h"

NTSTATUS Callouts::Init(DEVICE_OBJECT& wfpDeviceObject)
{
    for (size_t i{ 0 }; i != ARRAYSIZE(wfpObjects); ++i)
    {
        UINT32 calloutId{};
        const auto status = FwpsCalloutRegister(&wfpDeviceObject, &wfpObjects[i].m_callout, &calloutId);
        if (status != STATUS_SUCCESS)
        {
            TraceLoggingWrite(g_tracer,
                "FwpsCalloutRegister failed",
                TraceLoggingLevel(WINEVENT_LEVEL_ERROR),
                TraceLoggingValue(wfpObjects[i].m_callout.calloutKey, "callout guid"),
                TraceLoggingNTStatus(status));

            return status;
        }

        m_registeredCalloutIds[i] = calloutId;

        TraceLoggingWrite(g_tracer,
            "callout registered",
            TraceLoggingLevel(WINEVENT_LEVEL_INFO),
            TraceLoggingValue(wfpObjects[i].m_callout.calloutKey, "callout guid"),
            TraceLoggingValue(calloutId, "callout ID"));
    }

    return STATUS_SUCCESS;
}

Callouts::~Callouts()
{
    for (auto& id : m_registeredCalloutIds)
    {
        if (id)
        {
            const auto status = FwpsCalloutUnregisterById(id);
            NT_ASSERT(status == STATUS_SUCCESS);
            if (status != STATUS_SUCCESS)
            {
                TraceLoggingWrite(g_tracer,
                    "FwpsCalloutUnregisterById failed",
                    TraceLoggingLevel(WINEVENT_LEVEL_CRITICAL),
                    TraceLoggingValue(id, "Callout ID"),
                    TraceLoggingNTStatus(status));
            }

            id = 0;
        }
    }
}
