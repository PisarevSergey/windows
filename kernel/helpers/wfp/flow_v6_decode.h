#pragma once

#include "in_fixed_values_decode.h"

namespace wfp::decode
{
    template<>
    struct ValueType<FWPS_FIELDS_ALE_FLOW_ESTABLISHED_V6, FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_ALE_APP_ID>
    {
        using type = FWP_BYTE_BLOB*;
    };

    template<>
    struct ValueType<FWPS_FIELDS_ALE_FLOW_ESTABLISHED_V6, FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_ALE_USER_ID>
    {
        using type = FWP_BYTE_BLOB*;
    };

    template<>
    struct ValueType<FWPS_FIELDS_ALE_FLOW_ESTABLISHED_V6, FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_IP_LOCAL_ADDRESS>
    {
        using type = FWP_BYTE_ARRAY16*;
    };

    template<>
    struct ValueType<FWPS_FIELDS_ALE_FLOW_ESTABLISHED_V6, FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_IP_REMOTE_ADDRESS>
    {
        using type = FWP_BYTE_ARRAY16*;
    };

    template<>
    struct ValueType<FWPS_FIELDS_ALE_FLOW_ESTABLISHED_V6, FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_IP_LOCAL_ADDRESS_TYPE>
    {
        using type = UINT8;
    };

    template<>
    struct ValueType<FWPS_FIELDS_ALE_FLOW_ESTABLISHED_V6, FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_IP_DESTINATION_ADDRESS_TYPE>
    {
        using type = UINT8;
    };

    template<>
    struct ValueType<FWPS_FIELDS_ALE_FLOW_ESTABLISHED_V6, FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_IP_LOCAL_PORT>
    {
        using type = UINT16;
    };

    template<>
    struct ValueType<FWPS_FIELDS_ALE_FLOW_ESTABLISHED_V6, FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_IP_REMOTE_PORT>
    {
        using type = UINT16;
    };

    template<>
    struct ValueType<FWPS_FIELDS_ALE_FLOW_ESTABLISHED_V6, FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_IP_LOCAL_INTERFACE>
    {
        using type = UINT64*;
    };

    template<>
    struct ValueType<FWPS_FIELDS_ALE_FLOW_ESTABLISHED_V6, FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_DIRECTION>
    {
        using type = UINT32;
    };

    template<>
    consteval UINT16 LayerId<FWPS_FIELDS_ALE_FLOW_ESTABLISHED_V6>() { return FWPS_LAYER_ALE_FLOW_ESTABLISHED_V6; }

    consteval FWP_DATA_TYPE TypeForField(FWPS_FIELDS_ALE_FLOW_ESTABLISHED_V6 field)
    {
        switch (field)
        {
        case FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_ALE_APP_ID:
            return FWP_BYTE_BLOB_TYPE;
        case FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_ALE_USER_ID:
            return FWP_TOKEN_ACCESS_INFORMATION_TYPE;
        case FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_IP_LOCAL_ADDRESS:
        case FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_IP_REMOTE_ADDRESS:
            return FWP_BYTE_ARRAY16_TYPE;
        case FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_IP_LOCAL_ADDRESS_TYPE:
        case FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_IP_DESTINATION_ADDRESS_TYPE:
            return FWP_UINT8;
        case FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_IP_LOCAL_PORT:
        case FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_IP_REMOTE_PORT:
            return FWP_UINT16;
        case FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_DIRECTION:
            return FWP_UINT32;
        case FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_IP_LOCAL_INTERFACE:
            return FWP_UINT64;
        }
    }
}