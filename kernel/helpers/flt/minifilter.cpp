#include "minifilter.h"

#include <trace_logging/trace.h>

namespace flt
{
    void MinifilterDeleter::operator()(PFLT_FILTER filter) const
    {
        FltUnregisterFilter(filter);
    }

    AutoMinifilter CreateMinifilter(NTSTATUS& status, DRIVER_OBJECT& driverObject, const FLT_REGISTRATION& fltReg)
    {
        PFLT_FILTER minifilter{};
        status = FltRegisterFilter(&driverObject, &fltReg, &minifilter);
        if (!NT_SUCCESS(status))
        {
            TraceLoggingWrite(g_tracer, "FltRegisterFilter failed", TraceLoggingLevel(WINEVENT_LEVEL_ERROR), TraceLoggingNTStatus(status));
            return AutoMinifilter{};
        }

        return AutoMinifilter{ minifilter };
    }

    AutoMinifilter CreateMinifilter(NTSTATUS& status, DRIVER_OBJECT& driverObject, PFLT_FILTER_UNLOAD_CALLBACK unload)
    {
        const FLT_REGISTRATION fltReg
        {
            .Size = sizeof(fltReg),
            .Version = FLT_REGISTRATION_VERSION,
            .FilterUnloadCallback = unload
        };

        return CreateMinifilter(status, driverObject, fltReg);
    }
}
