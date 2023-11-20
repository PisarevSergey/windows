#pragma once

#include <ntddk.h>

#pragma warning(push)
#pragma warning(disable : 4471)
#include <wdf.h>
#pragma warning(pop)

namespace kmdf {
    WDF_DRIVER_CONFIG CreateDriverConfig(PFN_WDF_DRIVER_UNLOAD unload = nullptr, PFN_WDF_DRIVER_DEVICE_ADD addDevice = nullptr);
}