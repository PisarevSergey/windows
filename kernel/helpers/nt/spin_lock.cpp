#include "spin_lock.h"

namespace nt {
    KIRQL ReadWriteSpinLock::lock() {
        return ExAcquireSpinLockExclusive(&m_lock);
    }

    void ReadWriteSpinLock::unlock(KIRQL oldIrql) {
        ExReleaseSpinLockExclusive(&m_lock, oldIrql);
    }

    KIRQL ReadWriteSpinLock::lock_shared() {
        return ExAcquireSpinLockShared(&m_lock);
    }

    void ReadWriteSpinLock::unlock_shared(KIRQL oldIrql) {
        ExReleaseSpinLockShared(&m_lock, oldIrql);
    }
}