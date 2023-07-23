#pragma once

#include <fwpsk.h>

namespace wfp
{
    template <FWPS_FIELDS_ALE_FLOW_ESTABLISHED_V4 fieldType>
    struct ValueType;

    template<>
    struct ValueType<FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_LOCAL_ADDRESS>
    {
        using type = UINT32;
    };

    template<>
    struct ValueType<FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_REMOTE_ADDRESS>
    {
        using type = UINT32;
    };

    template<>
    struct ValueType<FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_ALE_APP_ID>
    {
        using type = FWP_BYTE_BLOB*;
    };

    template<>
    struct ValueType<FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_ALE_USER_ID>
    {
        using type = FWP_BYTE_BLOB*;
    };

    template<>
    struct ValueType<FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_LOCAL_ADDRESS_TYPE>
    {
        using type = UINT8;
    };

    template<>
    struct ValueType<FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_DESTINATION_ADDRESS_TYPE>
    {
        using type = UINT8;
    };

    template<>
    struct ValueType<FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_LOCAL_PORT>
    {
        using type = UINT16;
    };

    template<>
    struct ValueType<FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_REMOTE_PORT>
    {
        using type = UINT16;
    };

    consteval auto TypeForField(auto field)
    {
        switch (field)
        {
        case FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_ALE_APP_ID:
            return FWP_BYTE_BLOB_TYPE;
        case FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_ALE_USER_ID:
            return FWP_TOKEN_ACCESS_INFORMATION_TYPE;
        case FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_LOCAL_ADDRESS:
        case FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_REMOTE_ADDRESS:
            return FWP_UINT32;
        case FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_LOCAL_ADDRESS_TYPE:
        case FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_DESTINATION_ADDRESS_TYPE:
            return FWP_UINT8;
        case FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_LOCAL_PORT:
        case FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_REMOTE_PORT:
            return FWP_UINT16;
        }
    }

    template <FWPS_FIELDS_ALE_FLOW_ESTABLISHED_V4 fieldType>
    auto GetValue(NTSTATUS& status, const FWPS_INCOMING_VALUES& values)
    {
        using VT = ValueType<fieldType>::type;

        if (FWPS_LAYER_ALE_FLOW_ESTABLISHED_V4 != values.layerId)
        {
            status = STATUS_INVALID_PARAMETER;
            return VT{};
        }

        if (values.incomingValue[fieldType].value.type != TypeForField(fieldType))
        {
            status = STATUS_INVALID_PARAMETER;
            return VT{};
        }

        if (fieldType >= values.valueCount)
        {
            status = STATUS_INVALID_PARAMETER;
            return VT{};
        }

        VT resultValue{};
        RtlCopyMemory(&resultValue, &values.incomingValue[fieldType].value.uint8, sizeof(resultValue));
        status = STATUS_SUCCESS;
        return resultValue;
    }
}
