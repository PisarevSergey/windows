#pragma once

#include <kcpp/auto_ptr.h>

namespace nt
{
    struct DeviceDeleter final
    {
        void operator()(DEVICE_OBJECT* device) const;
    };

    using AutoDevice = kcpp::auto_ptr<DEVICE_OBJECT>;

    AutoDevice CreateDevice(NTSTATUS& status,
        DRIVER_OBJECT& driver,
        DEVICE_TYPE deviceType = FILE_DEVICE_UNKNOWN,
        ULONG deviceCharacteristics = FILE_DEVICE_SECURE_OPEN,
        BOOLEAN exclusive = FALSE,
        ULONG deviceExtensionSize = 0,
        const wchar_t* deviceName = nullptr);
}
