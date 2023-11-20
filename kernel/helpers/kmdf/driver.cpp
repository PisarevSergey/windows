#include "driver.h"

WDF_DRIVER_CONFIG kmdf::CreateDriverConfig(PFN_WDF_DRIVER_UNLOAD unload, PFN_WDF_DRIVER_DEVICE_ADD addDevice)
{
    WDF_DRIVER_CONFIG driverConfig;
    WDF_DRIVER_CONFIG_INIT(&driverConfig, addDevice);
    driverConfig.EvtDriverUnload = unload;
    return driverConfig;
}
