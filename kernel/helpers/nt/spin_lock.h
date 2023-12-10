#pragma once

#include <wdm.h>

#include <kcpp/mutex.h>

namespace nt {
    class ReadWriteSpinLock final {
    public:
        KIRQL lock();
        void unlock(KIRQL oldIrql);

        KIRQL lock_shared();
        void unlock_shared(KIRQL oldIrql);

        ReadWriteSpinLock(ReadWriteSpinLock&&) = delete;
    private:
        EX_SPIN_LOCK m_lock{};
    };
}

template <>
class kcpp::lock_guard<nt::ReadWriteSpinLock> final {

    enum class sharing {exclusive, shared};

public:
    explicit lock_guard(nt::ReadWriteSpinLock& lock, sharing sh = sharing::exclusive) : m_lock{ lock }, m_sharing{sh} {
        switch (m_sharing) {
        case sharing::exclusive:
            m_oldIrql = m_lock.lock();
            break;
        case sharing::shared:
            m_oldIrql = m_lock.lock_shared();
            break;
        };
    }

    ~lock_guard() {
        switch (m_sharing) {
        case sharing::exclusive:
            m_lock.unlock(m_oldIrql);
            break;
        case sharing::shared:
            m_lock.unlock_shared(m_oldIrql);
            break;
        };
    }

    lock_guard(lock_guard&&) = delete;
private:
    nt::ReadWriteSpinLock& m_lock;
    KIRQL m_oldIrql{};
    sharing m_sharing{};
};
