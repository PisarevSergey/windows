#pragma once

#include <kcpp/auto_ptr.h>

struct BfeCloser final {
    void operator()(HANDLE h) const;
};
using AutoBfe = kcpp::auto_ptr<HANDLE, BfeCloser>;

AutoBfe CreateBfeObjects(NTSTATUS& status);

struct BfeChangeHandleReleaser final {
    void operator()(HANDLE changeHandle) const;
};
using AutoBfeChangeHandle = kcpp::auto_ptr<HANDLE, BfeChangeHandleReleaser>;

class BfeStateSubscriber final {
public:
    BfeStateSubscriber() = default;

    [[nodiscard]]
    NTSTATUS Init(void* deviceObject, FWPM_SERVICE_STATE_CHANGE_CALLBACK0 callback, void* context = nullptr);

    BfeStateSubscriber(BfeStateSubscriber&&) = delete;
private:
    AutoBfeChangeHandle m_changeHandle;
};
