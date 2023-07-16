#pragma once

#include <wdm.h>

#include <kcpp/auto_ptr.h>

namespace nt
{
    template <typename T>
    struct ObjectDereferencer final
    {
        void operator()(T* object) const
        {
            ObDereferenceObject(object);
        }
    };

    using AutoReferencedDevice = kcpp::auto_ptr<DEVICE_OBJECT, ObjectDereferencer<DEVICE_OBJECT>>;
}
