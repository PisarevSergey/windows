#pragma once

#include <wdm.h>
#include <TraceLoggingProvider.h>
#include <winmeta.h>

TRACELOGGING_DECLARE_PROVIDER(g_tracer);

struct Tracer final
{
    Tracer();
    ~Tracer();

    Tracer(Tracer&&) = delete;
};
