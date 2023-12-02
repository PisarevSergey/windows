#include "device_property.h"

namespace kmdf {
    WDF_DEVICE_PROPERTY_DATA CreateDevicePropertyData(const DEVPROPKEY& propertyKey, LCID lcid, ULONG flags)
    {
        WDF_DEVICE_PROPERTY_DATA devPropertyData;
        WDF_DEVICE_PROPERTY_DATA_INIT(&devPropertyData, &propertyKey);
        devPropertyData.Lcid = lcid;
        devPropertyData.Flags = flags;
        return devPropertyData;
    }
}
