#include "pch.h"

namespace flow
{
    namespace v6
    {
        void classify([[maybe_unused]] const FWPS_INCOMING_VALUES0* inFixedValues,
            [[maybe_unused]] const FWPS_INCOMING_METADATA_VALUES0* inMetaValues,
            [[maybe_unused]] void* layerData,
            [[maybe_unused]] const void* classifyContext,
            [[maybe_unused]] const FWPS_FILTER3* filter,
            [[maybe_unused]] UINT64 flowContext,
            [[maybe_unused]] FWPS_CLASSIFY_OUT0* classifyOut)
        {
            //NTSTATUS status{};
            //const auto* appPathBlob = GetValue<FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_ALE_APP_ID>(status, *inFixedValues);
            //static_cast<void>(appPathBlob);
        }

        NTSTATUS notify([[maybe_unused]] FWPS_CALLOUT_NOTIFY_TYPE notifyType,
            [[maybe_unused]] const GUID* filterKey,
            [[maybe_unused]] FWPS_FILTER3* filter)
        {
            return STATUS_SUCCESS;
        }
    }

}
