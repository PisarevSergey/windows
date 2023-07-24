#pragma once

namespace
{
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
}

namespace wfp::decode
{
    template <typename DataFieldsEnum, DataFieldsEnum fieldType>
    struct ValueType;

    template<typename FieldIdType>
    consteval UINT16 LayerId();

    template <typename DataFieldsEnum, DataFieldsEnum fieldId>
    auto GetValue(NTSTATUS& status, const FWPS_INCOMING_VALUES& values)
    {
        using VT = ValueType<DataFieldsEnum, fieldId>::type;

        if (LayerId<decltype(fieldId)>() != values.layerId)
        {
            status = STATUS_INVALID_PARAMETER;
            return VT{};
        }

        if (values.incomingValue[fieldId].value.type != TypeForField(fieldId))
        {
            status = STATUS_INVALID_PARAMETER;
            return VT{};
        }

        if (fieldId >= values.valueCount)
        {
            status = STATUS_INVALID_PARAMETER;
            return VT{};
        }

        VT resultValue{};
        RtlCopyMemory(&resultValue, &values.incomingValue[fieldId].value.uint8, sizeof(resultValue));
        status = STATUS_SUCCESS;
        return resultValue;
    }
}

#define GET_VALUE(status, fieldId, inFixedValues) wfp::decode::GetValue<decltype(fieldId), fieldId>(status, *inFixedValues)
