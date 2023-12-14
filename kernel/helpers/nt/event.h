#pragma once

#include <wdm.h>

namespace nt {
    class Event final {
    public:
        explicit Event(EVENT_TYPE eventType, bool signalled = false);

        LONG Set(KPRIORITY increment = IO_NO_INCREMENT, bool wait = false);
        NTSTATUS Wait(PLARGE_INTEGER Timeout = nullptr, KWAIT_REASON WaitReason = Executive,
            KPROCESSOR_MODE WaitMode = KernelMode, bool alertable = false);

        Event(Event&&) = delete;
    private:
        KEVENT m_event;
    };
}