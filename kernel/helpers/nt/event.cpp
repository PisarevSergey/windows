#include "event.h"

namespace nt {
    Event::Event(EVENT_TYPE eventType, bool signalled) {
        KeInitializeEvent(&m_event, eventType, signalled ? TRUE : FALSE);
    }

    LONG Event::Set(KPRIORITY increment, bool wait) {
        return KeSetEvent(&m_event, increment, wait ? TRUE : FALSE);
    }

    NTSTATUS Event::Wait(PLARGE_INTEGER Timeout, KWAIT_REASON WaitReason, KPROCESSOR_MODE WaitMode, bool alertable) {
        return KeWaitForSingleObject(&m_event, WaitReason, WaitMode, alertable ? TRUE : FALSE, Timeout);
    }
}