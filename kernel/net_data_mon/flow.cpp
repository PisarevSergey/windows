#include "pch.h"

namespace flow
{
    namespace v4
    {
        void classify(const FWPS_INCOMING_VALUES0* inFixedValues,
            [[maybe_unused]] const FWPS_INCOMING_METADATA_VALUES0* inMetaValues,
            [[maybe_unused]] void* layerData,
            [[maybe_unused]] const void* classifyContext,
            [[maybe_unused]] const FWPS_FILTER3* filter,
            [[maybe_unused]] UINT64 flowContext,
            [[maybe_unused]] FWPS_CLASSIFY_OUT0* classifyOut)
        {
            const auto* fixedValues = inFixedValues->incomingValue;

            const auto& appPathValue = fixedValues[FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_ALE_APP_ID].value;
            if (appPathValue.type == FWP_BYTE_BLOB_TYPE)
            {
                const auto& appPathBlob = *appPathValue.byteBlob;
                TraceInfo("app path", TraceLoggingWideString(reinterpret_cast<const wchar_t*>(appPathBlob.data), "app path"));
            }
            else
            {
                TraceWarning("no app path");
            }

            const auto& localUserValue = fixedValues[FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_ALE_USER_ID].value;
            if (localUserValue.type == FWP_TOKEN_ACCESS_INFORMATION_TYPE)
            {
                const auto& localUserBlob = localUserValue.byteBlob;
                const auto* accessToken = reinterpret_cast<TOKEN_ACCESS_INFORMATION*>(localUserBlob->data);
                const auto& sidHash = *accessToken->SidHash;
                TraceInfo("user", TraceLoggingSid(static_cast<SID*>(sidHash.SidAttr->Sid), "user"));
            }
            else
            {
                TraceWarning("no user");
            }
        }

        NTSTATUS notify(FWPS_CALLOUT_NOTIFY_TYPE notifyType,
            const GUID* filterKey,
            [[maybe_unused]] FWPS_FILTER3* filter)
        {
            switch (notifyType)
            {
            case FWPS_CALLOUT_NOTIFY_ADD_FILTER:
                NT_ASSERT(filterKey);
                TraceInfo("adding filter", TraceLoggingValue(*filterKey));
                break;
            case FWPS_CALLOUT_NOTIFY_DELETE_FILTER:
                NT_ASSERT(nullptr == filterKey);
                TraceInfo("removing filter");
                break;
            default:
                TraceInfo("unknown notification", TraceLoggingValue(static_cast<int>(notifyType)));
            }

            return STATUS_SUCCESS;
        }
    }

    namespace v6
    {
        void classify([[maybe_unused]] const FWPS_INCOMING_VALUES0* inFixedValues,
            [[maybe_unused]] const FWPS_INCOMING_METADATA_VALUES0* inMetaValues,
            [[maybe_unused]] void* layerData,
            [[maybe_unused]] const void* classifyContext,
            [[maybe_unused]] const FWPS_FILTER3* filter,
            [[maybe_unused]] UINT64 flowContext,
            [[maybe_unused]] FWPS_CLASSIFY_OUT0* classifyOut)
        {}

        NTSTATUS notify([[maybe_unused]] FWPS_CALLOUT_NOTIFY_TYPE notifyType,
            [[maybe_unused]] const GUID* filterKey,
            [[maybe_unused]] FWPS_FILTER3* filter)
        {
            return STATUS_SUCCESS;
        }
    }
}
