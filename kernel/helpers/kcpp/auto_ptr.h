#pragma once

namespace kcpp
{
    template <typename T>
    struct default_deleter final
    {
        void operator()(T* ptr) const
        {
            delete ptr;
        }
    };

    template <typename T, typename D = default_deleter<T>>
    class auto_ptr_base
    {
    public:
        void reset(T* ptr = nullptr)
        {
            destroy();
            m_ptr = ptr;
        }

        [[nodiscard]] auto release()
        {
            auto tmp = m_ptr;
            m_ptr = nullptr;
            return tmp;
        }

        T* get() { return m_ptr; }
        const T* get() const { return m_ptr; }

        explicit operator T** () { return &m_ptr; }

    protected:
        explicit auto_ptr_base(T* ptr) : m_ptr{ ptr }
        {}

        ~auto_ptr_base()
        {
            destroy();
        }

        auto_ptr_base(auto_ptr_base&& rhs) noexcept : m_ptr{ rhs.release() }, m_deleter{ static_cast<D&&>(rhs.m_deleter) }
        {}

        auto_ptr_base& operator=(auto_ptr_base&& rhs) noexcept
        {
            if (this == &rhs)
            {
                return *this;
            }

            reset(rhs.release());
            m_deleter = static_cast<D&&>(rhs.m_deleter);

            return *this;
        }
    private:
        T* m_ptr{};
        D m_deleter{};

        void destroy()
        {
            if (m_ptr)
            {
                m_deleter(m_ptr);
                m_ptr = nullptr;
            }
        }
    };

    template <typename T, typename D = default_deleter<T>>
    class auto_ptr final : public auto_ptr_base<T, D>
    {
        using base = auto_ptr_base<T, D>;
    public:
        explicit auto_ptr(T* ptr = nullptr) : base{ ptr }
        {}

        auto_ptr(auto_ptr&& rhs) noexcept : base{ rhs }
        {}

        auto_ptr& operator=(auto_ptr&& rhs) noexcept
        {
            base::operator=(static_cast<auto_ptr&&>(rhs));
            return *this;
        }

        T* operator->() { return base::get(); }
        const T* operator->() const { return base::get(); }

        T& operator*() { return *base::get(); }
        const T& operator*() const { return *base::get(); }
    };

    template <typename T, typename D>
    class auto_ptr<T*, D> final : public auto_ptr_base<T, D>
    {
        using base = auto_ptr_base<T, D>;
    public:
        explicit auto_ptr(T* ptr = nullptr) : base{ ptr }
        {}

        auto_ptr& operator=(auto_ptr&& rhs) noexcept
        {
            base::operator=(static_cast<auto_ptr&&>(rhs));
            return *this;
        }

        auto_ptr(auto_ptr&& rhs) noexcept : base{ rhs }
        {}
    };
}
