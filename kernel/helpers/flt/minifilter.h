#pragma once

#include <fltKernel.h>

#include <kcpp/auto_ptr.h>

namespace flt
{
    struct MinifilterDeleter final
    {
        void operator()(PFLT_FILTER filter) const;
    };

    using AutoMinifilter = kcpp::auto_ptr<PFLT_FILTER, MinifilterDeleter>;

    AutoMinifilter CreateMinifilter(NTSTATUS& status, DRIVER_OBJECT& driverObject, const FLT_REGISTRATION& fltReg);
    AutoMinifilter CreateMinifilter(NTSTATUS& status, DRIVER_OBJECT& driverObject, PFLT_FILTER_UNLOAD_CALLBACK unload);
}
