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
    consteval UINT16 LayerId<FWPS_FIELDS_ALE_FLOW_ESTABLISHED_V6>() { return FWPS_LAYER_ALE_FLOW_ESTABLISHED_V6; }

    consteval FWP_DATA_TYPE TypeForField(FWPS_FIELDS_ALE_FLOW_ESTABLISHED_V6 field)
    {
        switch (field)
        {
        case FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_ALE_APP_ID:
            return FWP_BYTE_BLOB_TYPE;
        }
    }
}