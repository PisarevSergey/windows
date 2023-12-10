#pragma once

namespace helpers {
    UNICODE_STRING ToStringView(NTSTATUS& status, WDFMEMORY stringMemory);
}