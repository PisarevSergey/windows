#include "spin_lock.h"

namespace nt {
    KIRQL ReadWriteSpinLock::AcquireExclusive() {
        return ExAcquireSpinLockExclusive(&m_lock);
    }

    void ReadWriteSpinLock::ReleaseExclusive(KIRQL oldIrql) {
        ExReleaseSpinLockExclusive(&m_lock, oldIrql);
    }

    KIRQL ReadWriteSpinLock::AcquireShared() {
        return ExAcquireSpinLockShared(&m_lock);
    }

    void ReadWriteSpinLock::ReleaseShared(KIRQL oldIrql) {
        ExReleaseSpinLockShared(&m_lock, oldIrql);
    }
}