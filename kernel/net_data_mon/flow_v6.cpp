#include "pch.h"

#include <wfp/flow_v6_decode.h>

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
            NTSTATUS status{};
            const auto* appPathBlob = GET_VALUE(status, FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_ALE_APP_ID, inFixedValues);
            if (!NT_SUCCESS(status))
            {
                TraceError("failed to get app path", TraceLoggingNTStatus(status));
                return;
            }
            TraceInfo("app path", TraceLoggingWideString(reinterpret_cast<const wchar_t*>(appPathBlob->data), "app path"));

            const auto* userBlob = GET_VALUE(status, FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_ALE_USER_ID, inFixedValues);
            const auto* accessInformation = reinterpret_cast<const TOKEN_ACCESS_INFORMATION*>(userBlob->data);
            TraceInfo("user", TraceLoggingSid(static_cast<SID*>(accessInformation->SidHash->SidAttr->Sid), "user"));

            const auto* localAddress = GET_VALUE(status, FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_IP_LOCAL_ADDRESS, inFixedValues);
            if (!NT_SUCCESS(status))
            {
                TraceError("failed to get local address", TraceLoggingNTStatus(status));
                return;
            }
            TraceInfo("local address", TraceLoggingIPv6Address(localAddress->byteArray16, "local address"));
        }

        NTSTATUS notify([[maybe_unused]] FWPS_CALLOUT_NOTIFY_TYPE notifyType,
            [[maybe_unused]] const GUID* filterKey,
            [[maybe_unused]] FWPS_FILTER3* filter)
        {
            return STATUS_SUCCESS;
        }
    }

}
