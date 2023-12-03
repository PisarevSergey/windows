#pragma once

#include <wdm.h>

namespace nt {
    class ReadWriteSpinLock final {
    public:
        KIRQL AcquireExclusive();
        void ReleaseExclusive(KIRQL oldIrql);

        KIRQL AcquireShared();
        void ReleaseShared(KIRQL oldIrql);

        ReadWriteSpinLock(ReadWriteSpinLock&&) = delete;
    private:
        EX_SPIN_LOCK m_lock{};
    };
}