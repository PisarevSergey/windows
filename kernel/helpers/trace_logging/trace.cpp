#include "trace.h"

Tracer::Tracer()
{
#if DBG
    const auto status =
#endif
    TraceLoggingRegister(g_tracer);

    NT_ASSERT(NT_SUCCESS(status));
}

Tracer::~Tracer()
{
    TraceLoggingUnregister(g_tracer);
}
