#include "device_property.h"
#include "object.h"

#include <trace_logging\trace.h>

namespace kmdf {
    WDF_DEVICE_PROPERTY_DATA CreateDevicePropertyData(const DEVPROPKEY& propertyKey, LCID lcid, ULONG flags)
    {
        WDF_DEVICE_PROPERTY_DATA devPropertyData;
        WDF_DEVICE_PROPERTY_DATA_INIT(&devPropertyData, &propertyKey);
        devPropertyData.Lcid = lcid;
        devPropertyData.Flags = flags;
        return devPropertyData;
    }

    WDFMEMORY AllocAndQueryDeviceProperty(NTSTATUS& status, WDFDEVICE device, const DEVPROPKEY& propertyKey, WDFOBJECT propertyMemoryParent, PFN_WDF_OBJECT_CONTEXT_CLEANUP propertyMemoryCleanupCallback)
    {
        auto deviceProperty = kmdf::CreateDevicePropertyData(propertyKey);
        auto memoryAttributes = kmdf::CreateObjectAttributes(propertyMemoryParent, propertyMemoryCleanupCallback);

        WDFMEMORY devicePropertyMemory{};
        DEVPROPTYPE propertyType{};
        status = WdfDeviceAllocAndQueryPropertyEx(device, &deviceProperty, PagedPool, &memoryAttributes, &devicePropertyMemory, &propertyType);
        if (STATUS_SUCCESS != status) {
            TraceError("WdfDeviceAllocAndQueryPropertyEx failed", TraceLoggingNTStatus(status));
            return {};
        }

        return devicePropertyMemory;
    }
}
