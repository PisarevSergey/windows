#pragma once

#include <ntddk.h>

#pragma warning(push)
#pragma warning(disable : 4471)
#include <wdf.h>
#pragma warning(pop)

namespace kmdf {
    [[nodiscard]]
    WDF_OBJECT_ATTRIBUTES CreateObjectAttributes(WDFOBJECT parent = nullptr, PFN_WDF_OBJECT_CONTEXT_CLEANUP cleanup = nullptr);
}