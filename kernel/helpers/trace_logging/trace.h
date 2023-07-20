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

#define Trace(level, eventName, ...) \
    TraceLoggingWrite(g_tracer, eventName, TraceLoggingLevel(level), __VA_ARGS__)

#define TraceInfo(eventName, ...) \
    Trace(WINEVENT_LEVEL_INFO, eventName, __VA_ARGS__)

#define TraceError(eventName, ...) \
    Trace(WINEVENT_LEVEL_ERROR, eventName, __VA_ARGS__)

#define TraceWarning(eventName, ...) \
    Trace(WINEVENT_LEVEL_WARNING, eventName, __VA_ARGS__)

#define TraceCritical(eventName, ...) \
    Trace(WINEVENT_LEVEL_CRITICAL, eventName, __VA_ARGS__)
