#include "pch.h"

#include <wfp/classify_decode.h>

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
            NTSTATUS status{};
            const auto* appPathBlob = wfp::GetValue<FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_ALE_APP_ID>(status, *inFixedValues);
            if (!NT_SUCCESS(status))
            {
                TraceError("failed to get app path", TraceLoggingNTStatus(status));
                return;
            }
            TraceInfo("app path", TraceLoggingWideString(reinterpret_cast<const wchar_t*>(appPathBlob->data), "app path"));

            const auto* localUserBlob = wfp::GetValue<FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_ALE_USER_ID>(status, *inFixedValues);
            if (!NT_SUCCESS(status))
            {
                TraceError("failed to get user", TraceLoggingNTStatus(status));
                return;
            }
            const auto* accessToken = reinterpret_cast<TOKEN_ACCESS_INFORMATION*>(localUserBlob->data);
            const auto& sidHash = *accessToken->SidHash;
            TraceInfo("user", TraceLoggingSid(static_cast<SID*>(sidHash.SidAttr->Sid), "user"));

            const auto localAddress = RtlUlongByteSwap(wfp::GetValue<FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_LOCAL_ADDRESS>(status, *inFixedValues));
            if (!NT_SUCCESS(status))
            {
                TraceError("failed to get local address", TraceLoggingNTStatus(status));
                return;
            }
            TraceInfo("local address", TraceLoggingIPv4Address(localAddress, "local address"));
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
