#include "mutex.h"

namespace nt {
    FastMutex::FastMutex() {
        ExInitializeFastMutex(&m_mutex);
    }

    void FastMutex::lock() {
        ExAcquireFastMutex(&m_mutex);
    }

    void FastMutex::unlock() {
        ExReleaseFastMutex(&m_mutex);
    }
}

