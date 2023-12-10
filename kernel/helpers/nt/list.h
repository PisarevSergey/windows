#pragma once

#include <wdm.h>

namespace nt {
    template <typename T>
    class List final{
    public:
        void InsertTail(T& entry) { InsertTailList(&head, &entry); }

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

        List(List&&) = delete;
    private:
        LIST_ENTRY m_head{ &m_head, &m_head };
    };
}