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
}
