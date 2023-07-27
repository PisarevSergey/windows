#include "util.h"

consteval const UNICODE_STRING operator ""_US(const wchar_t* string, size_t size)
{
    return UNICODE_STRING
    {
        .Length = USHORT(2 * size ), .MaximumLength = USHORT(2 * size), .Buffer = const_cast<wchar_t*>(string)
    };
}

const UNICODE_STRING& wfp::util::AddressTypeToString(NL_ADDRESS_TYPE addressType)
{
    switch (addressType)
    {
        case NlatUnspecified:
        {
            static constexpr auto str = L"NlatUnspecified"_US;
            return str;
        }
        case NlatUnicast:
        {
            static constexpr auto str = L"NlatUnicast"_US;
            return str;
        }
        case NlatAnycast:
        {
            static constexpr auto str = L"NlatAnycast"_US;
            return str;
        }
        case NlatMulticast:
        {
            static constexpr auto str = L"NlatMulticast"_US;
            return str;
        }
        case NlatBroadcast:
        {
            static constexpr auto str = L"NlatBroadcast"_US;
            return str;
        }
        case NlatInvalid:
        {
            static constexpr auto str = L"NlatInvalid"_US;
            return str;
        }
        default:
        {
            static constexpr auto str = L"unknown address type"_US;
            return str;
        }
    }
}

const UNICODE_STRING& wfp::util::DirectionToString(FWP_DIRECTION direction)
{
    switch (direction)
    {
    case FWP_DIRECTION_OUTBOUND:
    {
        static constexpr auto str = L"FWP_DIRECTION_OUTBOUND"_US;
        return str;
    }
    case FWP_DIRECTION_INBOUND:
    {
        static constexpr auto str = L"FWP_DIRECTION_INBOUND"_US;
        return str;
    }
    case FWP_DIRECTION_MAX:
    {
        NT_ASSERT(FALSE);
        static constexpr auto str = L"FWP_DIRECTION_MAX"_US;
        return str;
    }
    default:
    {
        NT_ASSERT(FALSE);
        static constexpr auto str = L"unknown direction"_US;
        return str;
    }
    }
}
