#pragma once

#include <wdm.h>

namespace nt {
    class FastMutex final
    {
    public:
        FastMutex();

        void lock();
        void unlock();

        FastMutex(FastMutex&&) = delete;
    private:
        FAST_MUTEX m_mutex;
    };
}