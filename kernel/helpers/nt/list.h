#pragma once

#include <wdm.h>

namespace nt {
    template <typename T>
    class List final{
    public:
        List() = default;

        void InsertTail(T& entry) { InsertTailList(&m_head, &entry); }

        T* RemoveHead() {
            auto entry = RemoveHeadList(&m_head);
            return (entry == &m_head) ? nullptr : static_cast<T*>(entry);
        }

        T* RemoveTail() {
            auto entry = RemoveTailList(&m_head);
            return (entry == &m_head) ? nullptr : static_cast<T*>(entry);
        }

        bool Remove(T& entry) {
            return RemoveEntryList(&entry) ? true : false;
        }

        bool IsEmpty() const { return IsListEmpty(&m_head) ? true : false; }

        decltype(auto) begin() const { return Iterator{ static_cast<const T*>(m_head.Flink) }; }
        decltype(auto) end() const { return Iterator{ static_cast<const T*>(&m_head)}; }

        decltype(auto) begin() { return Iterator{ static_cast<T*>(m_head.Flink) }; }
        decltype(auto) end() { return Iterator{ static_cast<T*>(&m_head)}; }

        List(List&&) = delete;

        struct Iterator {
            explicit Iterator(T* ptr) : m_ptr{ ptr } {}

            T& operator*() { return *m_ptr; }
            const T& operator*() const { return *m_ptr; }

            T* operator->() { return m_ptr; }
            const T* operator->() const { return m_ptr; }

            Iterator& operator++() { m_ptr = static_cast<T*>(m_ptr->Flink); return *this; }

            friend bool operator== (const Iterator& a, const Iterator& b) { return a.m_ptr == b.m_ptr; };
            friend bool operator!= (const Iterator& a, const Iterator& b) { return a.m_ptr != b.m_ptr; };

        private:
            T* m_ptr{};
        };

    private:
        LIST_ENTRY m_head{ &m_head, &m_head };
    };
}