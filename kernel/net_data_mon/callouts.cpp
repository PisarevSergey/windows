#include "pch.h"

NTSTATUS Callouts::Init(DEVICE_OBJECT& wfpDeviceObject)
{
    for (size_t i{ 0 }; i != ARRAYSIZE(wfpObjects); ++i)
    {
        UINT32 calloutId{};
        const auto status = FwpsCalloutRegister(&wfpDeviceObject, &wfpObjects[i].m_callout, &calloutId);
        if (status != STATUS_SUCCESS)
        {
            TraceError("FwpsCalloutRegister failed", TraceLoggingValue(wfpObjects[i].m_callout.calloutKey), TraceLoggingNTStatus(status));

            return status;
        }

        m_registeredCalloutIds[i] = calloutId;

        TraceInfo("callout registered", TraceLoggingValue(wfpObjects[i].m_callout.calloutKey), TraceLoggingValue(calloutId));
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
                TraceCritical("FwpsCalloutUnregisterById failed", TraceLoggingValue(id, "Callout ID"), TraceLoggingNTStatus(status));
            }

            id = 0;
        }
    }
}
