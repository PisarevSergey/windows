#include "pch.h"

namespace helpers {
    UNICODE_STRING ToStringView(NTSTATUS& status, WDFMEMORY stringMemory) {
        size_t bufferSize{};
        auto buffer = static_cast<wchar_t*>(WdfMemoryGetBuffer(stringMemory, &bufferSize));

        USHORT bufferSizeUshort{};
        status = RtlULongLongToUShort(bufferSize, &bufferSizeUshort);
        if (!NT_SUCCESS(status)) {
            TraceCritical("RtlULongLongToUShort failed", TraceLoggingValue(bufferSize));
            return {};
        }

        size_t stringLength{};
        status = RtlStringCbLengthW(buffer, bufferSize, &stringLength);
        if (!NT_SUCCESS(status)) {
            TraceCritical("RtlStringCbLengthW failed to calculate string length",
                TraceLoggingWideString(buffer),
                TraceLoggingValue(bufferSize),
                TraceLoggingNTStatus(status));
            return {};
        }

        USHORT stringLengthUshort{};
        status = RtlULongLongToUShort(stringLength, &stringLengthUshort);
        if (!NT_SUCCESS(status)) {
            TraceCritical("RtlULongLongToUShort failed", TraceLoggingValue(stringLength));
            return {};
        }

        return UNICODE_STRING{ .Length = stringLengthUshort, .MaximumLength = bufferSizeUshort, .Buffer = buffer };
    }
}