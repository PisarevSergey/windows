#pragma once

namespace kcpp
{
    template <typename T>
    class scope_guard final
    {
    public:
        scope_guard(T&& releaser) : m_releaser{ static_cast<T&&>(releaser) }
        {}

        ~scope_guard()
        {
            if (m_willRelease)
            {
                m_releaser();
            }
        }

        void release() { m_willRelease = false; }

        scope_guard(scope_guard&&) = delete;
    private:
        T m_releaser;
        bool m_willRelease{ true };
    };
}
