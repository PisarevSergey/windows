#pragma once

#include <wdm.h>

#include <kcpp/mutex.h>

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

template<>
class kcpp::lock_guard<nt::FastMutex> {
public:
    lock_guard(nt::FastMutex& mutex) : m_mutex{mutex} {
        m_mutex.lock();
    }

    ~lock_guard() {
        m_mutex.unlock();
    }
private:
    nt::FastMutex& m_mutex;
};