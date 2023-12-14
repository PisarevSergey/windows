#include "remove_lock.h"

namespace nt {

RemoveLock::RemoveLock(ULONG tag, ULONG maxMin, ULONG highWater) {
    IoInitializeRemoveLock(&m_lock, tag, maxMin, highWater);
}

NTSTATUS RemoveLock::Acquire(void* tag) {
    return IoAcquireRemoveLock(&m_lock, tag);
}

void RemoveLock::Release(void* tag) {
    IoReleaseRemoveLock(&m_lock, tag);
}

void RemoveLock::ReleaseAndWait(void* tag) {
    IoReleaseRemoveLockAndWait(&m_lock, tag);
}

}
