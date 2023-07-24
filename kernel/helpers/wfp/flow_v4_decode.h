#pragma once

#include "in_fixed_values_decode.h"

namespace wfp::decode
{
    template<>
    struct ValueType<FWPS_FIELDS_ALE_FLOW_ESTABLISHED_V4, FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_LOCAL_ADDRESS>
    {
        using type = UINT32;
    };

    template<>
    struct ValueType<FWPS_FIELDS_ALE_FLOW_ESTABLISHED_V4, FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_REMOTE_ADDRESS>
    {
        using type = UINT32;
    };

    template<>
    struct ValueType<FWPS_FIELDS_ALE_FLOW_ESTABLISHED_V4, FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_ALE_APP_ID>
    {
        using type = FWP_BYTE_BLOB*;
    };

    template<>
    struct ValueType<FWPS_FIELDS_ALE_FLOW_ESTABLISHED_V4, FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_ALE_USER_ID>
    {
        using type = FWP_BYTE_BLOB*;
    };

    template<>
    struct ValueType<FWPS_FIELDS_ALE_FLOW_ESTABLISHED_V4, FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_LOCAL_ADDRESS_TYPE>
    {
        using type = UINT8;
    };

    template<>
    struct ValueType<FWPS_FIELDS_ALE_FLOW_ESTABLISHED_V4, FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_DESTINATION_ADDRESS_TYPE>
    {
        using type = UINT8;
    };

    template<>
    struct ValueType<FWPS_FIELDS_ALE_FLOW_ESTABLISHED_V4, FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_LOCAL_PORT>
    {
        using type = UINT16;
    };

    template<>
    struct ValueType<FWPS_FIELDS_ALE_FLOW_ESTABLISHED_V4, FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_REMOTE_PORT>
    {
        using type = UINT16;
    };

    template<>
    consteval UINT16 LayerId<FWPS_FIELDS_ALE_FLOW_ESTABLISHED_V4>() { return FWPS_LAYER_ALE_FLOW_ESTABLISHED_V4; }
}
