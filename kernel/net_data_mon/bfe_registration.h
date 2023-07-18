#pragma once

#include <kcpp/auto_ptr.h>

struct BfeCloser final
{
    void operator()(HANDLE h) const
    {
        FwpmEngineClose(h);
    }
};

using AutoBfe = kcpp::auto_ptr<HANDLE, BfeCloser>;

AutoBfe CreateBfeObjects(NTSTATUS& status);