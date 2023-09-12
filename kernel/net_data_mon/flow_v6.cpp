#include "pch.h"

#include <wfp/flow_v6_decode.h>

void flow::v6::classify(const FWPS_INCOMING_VALUES0* inFixedValues,
    [[maybe_unused]] const FWPS_INCOMING_METADATA_VALUES0* inMetaValues,
    [[maybe_unused]] void* layerData,
    [[maybe_unused]] const void* classifyContext,
    [[maybe_unused]] const FWPS_FILTER3* filter,
    [[maybe_unused]] UINT64 flowContext,
    [[maybe_unused]] FWPS_CLASSIFY_OUT0* classifyOut)
{
    using namespace wfp::util;

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

    const auto localAddressType = NL_ADDRESS_TYPE(GET_VALUE(status, FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_IP_LOCAL_ADDRESS_TYPE, inFixedValues));
    if (!NT_SUCCESS(status))
    {
        TraceError("failed to get llocal address type", TraceLoggingNTStatus(status));
        return;
    }
    TraceInfo("local address type", TraceLoggingUnicodeString(&AddressTypeToString(localAddressType), "local address type"));

    const auto localPort = RtlUshortByteSwap(GET_VALUE(status, FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_IP_LOCAL_PORT, inFixedValues));
    if (!NT_SUCCESS(status))
    {
        TraceError("failed to get local port", TraceLoggingNTStatus(status));
        return;
    }
    TraceInfo("local port", TraceLoggingPort(localPort, "local port"));

    const auto* remoteAddress = GET_VALUE(status, FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_IP_REMOTE_ADDRESS, inFixedValues);
    if (!NT_SUCCESS(status))
    {
        TraceError("failed to get remote address", TraceLoggingNTStatus(status));
        return;
    }
    TraceInfo("remote address", TraceLoggingIPv6Address(remoteAddress->byteArray16, "remote address"));

    const auto remotePort = RtlUshortByteSwap(GET_VALUE(status, FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_IP_REMOTE_PORT, inFixedValues));
    if (!NT_SUCCESS(status))
    {
        TraceError("failed to get remote port", TraceLoggingNTStatus(status));
        return;
    }
    TraceInfo("remote port", TraceLoggingPort(remotePort, "remote port"));

    const auto remoteAddressType = NL_ADDRESS_TYPE(GET_VALUE(status, FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_IP_DESTINATION_ADDRESS_TYPE, inFixedValues));
    if (!NT_SUCCESS(status))
    {
        TraceError("failed to get destination address type", TraceLoggingNTStatus(status));
        return;
    }
    TraceInfo("destination address type", TraceLoggingUnicodeString(&AddressTypeToString(remoteAddressType), "remote address type"));

    const auto* localInterfaceLuid = GET_VALUE(status, FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_IP_LOCAL_INTERFACE, inFixedValues);
    if (!NT_SUCCESS(status))
    {
        TraceError("failed to get local interface LUID", TraceLoggingNTStatus(status));
        return;
    }
    NT_ASSERT(localInterfaceLuid);
    TraceInfo("local interface LUID", TraceLoggingValue(*localInterfaceLuid, "local interface LUID"));

    const auto direction = FWP_DIRECTION(GET_VALUE(status, FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_DIRECTION, inFixedValues));
    if (!NT_SUCCESS(status))
    {
        TraceError("failed to get direction", TraceLoggingNTStatus(status));
        return;
    }
    TraceInfo("direction", TraceLoggingUnicodeString(&DirectionToString(direction), "direction"));
}

NTSTATUS flow::v6::notify(FWPS_CALLOUT_NOTIFY_TYPE notifyType,
    const GUID* filterKey,
    FWPS_FILTER3* filter)
{
    switch (notifyType)
    {
    case FWPS_CALLOUT_NOTIFY_ADD_FILTER:
        NT_ASSERT(filterKey);
        TraceInfo("adding filter", TraceLoggingValue(*filterKey, "filter GUID"));
        break;
    case FWPS_CALLOUT_NOTIFY_DELETE_FILTER:
        NT_ASSERT(nullptr == filterKey);
        TraceInfo("removing filter", TraceLoggingValue(filter->filterId, "filter ID"));
        break;
    default:
        TraceInfo("unknown notification", TraceLoggingValue(static_cast<int>(notifyType), "notification value"));
    }

    return STATUS_SUCCESS;
}
