#pragma once

#include <wdm.h>

namespace nt {

    class RemoveLock final {
    public:
        explicit RemoveLock(ULONG tag,
            ULONG maxMin = 0,
            ULONG highWater = 0);

        [[nodiscard]]
        NTSTATUS Acquire(void* tag);
        void Release(void* tag);
        void ReleaseAndWait(void* tag);

        RemoveLock(RemoveLock&&) = delete;
    private:
        IO_REMOVE_LOCK m_lock;
    };

}