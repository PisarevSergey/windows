#pragma once

#include <ntddk.h>

#pragma warning(push)
#pragma warning(disable : 4471)
#include <wdf.h>
#pragma warning(pop)

namespace kmdf {
    WDF_DEVICE_PROPERTY_DATA CreateDevicePropertyData(const DEVPROPKEY& propertyKey, LCID lcid = LOCALE_NEUTRAL, ULONG flags = 0);
    WDFMEMORY AllocAndQueryDeviceProperty(NTSTATUS& status, WDFDEVICE device, const DEVPROPKEY& propertyKey, WDFOBJECT propertyMemoryParent = nullptr, PFN_WDF_OBJECT_CONTEXT_CLEANUP propertyMemoryCleanupCallback = nullptr);
}