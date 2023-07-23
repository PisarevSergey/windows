#include "pch.h"

#include <wfp/flow_v4_decode.h>

using wfp::flow_v4_decode::GetValue;

void flow::v4::classify(const FWPS_INCOMING_VALUES0* inFixedValues,
    [[maybe_unused]] const FWPS_INCOMING_METADATA_VALUES0* inMetaValues,
    [[maybe_unused]] void* layerData,
    [[maybe_unused]] const void* classifyContext,
    [[maybe_unused]] const FWPS_FILTER3* filter,
    [[maybe_unused]] UINT64 flowContext,
    [[maybe_unused]] FWPS_CLASSIFY_OUT0* classifyOut)
{
    NTSTATUS status{};
    const auto* appPathBlob = GetValue<FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_ALE_APP_ID>(status, *inFixedValues);
    if (!NT_SUCCESS(status))
    {
        TraceError("failed to get app path", TraceLoggingNTStatus(status));
        return;
    }
    TraceInfo("app path", TraceLoggingWideString(reinterpret_cast<const wchar_t*>(appPathBlob->data), "app path"));

    const auto* localUserBlob = GetValue<FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_ALE_USER_ID>(status, *inFixedValues);
    if (!NT_SUCCESS(status))
    {
        TraceError("failed to get user", TraceLoggingNTStatus(status));
        return;
    }
    const auto* accessToken = reinterpret_cast<TOKEN_ACCESS_INFORMATION*>(localUserBlob->data);
    const auto& sidHash = *accessToken->SidHash;
    TraceInfo("user", TraceLoggingSid(static_cast<SID*>(sidHash.SidAttr->Sid), "user"));

    const auto localAddress = RtlUlongByteSwap(GetValue<FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_LOCAL_ADDRESS>(status, *inFixedValues));
    if (!NT_SUCCESS(status))
    {
        TraceError("failed to get local address", TraceLoggingNTStatus(status));
        return;
    }
    TraceInfo("local address", TraceLoggingIPv4Address(localAddress, "local address"));

    const auto localPort = RtlUshortByteSwap(GetValue<FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_LOCAL_PORT>(status, *inFixedValues));
    if (!NT_SUCCESS(status))
    {
        TraceError("failed to get local port", TraceLoggingNTStatus(status));
        return;
    }
    TraceInfo("local port", TraceLoggingPort(localPort, "local port"));

    const auto remoteAddress = RtlUlongByteSwap(GetValue<FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_REMOTE_ADDRESS>(status, *inFixedValues));
    if (!NT_SUCCESS(status))
    {
        TraceError("failed to get remote address", TraceLoggingNTStatus(status));
        return;
    }
    TraceInfo("remote address", TraceLoggingIPv4Address(remoteAddress, "remote address"));

    const auto remotePort = RtlUshortByteSwap(GetValue<FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_REMOTE_PORT>(status, *inFixedValues));
    if (!NT_SUCCESS(status))
    {
        TraceError("failed to get remote port", TraceLoggingNTStatus(status));
        return;
    }
    TraceInfo("remote port", TraceLoggingPort(remotePort, "remote port"));
}

NTSTATUS flow::v4::notify(FWPS_CALLOUT_NOTIFY_TYPE notifyType,
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
