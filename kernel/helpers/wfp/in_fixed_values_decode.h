#pragma once

namespace wfp::decode
{
    template <typename DataFieldsEnum, DataFieldsEnum fieldType>
    struct ValueType;

    template<typename FieldIdType>
    consteval UINT16 LayerId();

    consteval FWP_DATA_TYPE TypeForField(FWPS_FIELDS_ALE_FLOW_ESTABLISHED_V4 field);
    consteval FWP_DATA_TYPE TypeForField(FWPS_FIELDS_ALE_FLOW_ESTABLISHED_V6 field);

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
