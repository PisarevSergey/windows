#pragma once

namespace flow::v6
{
    void classify(const FWPS_INCOMING_VALUES0* inFixedValues,
        const FWPS_INCOMING_METADATA_VALUES0* inMetaValues,
        void* layerData,
        const void* classifyContext,
        const FWPS_FILTER3* filter,
        UINT64 flowContext,
        FWPS_CLASSIFY_OUT0* classifyOut);

    NTSTATUS notify(FWPS_CALLOUT_NOTIFY_TYPE notifyType,
        const GUID* filterKey,
        FWPS_FILTER3* filter);

    namespace management
    {
        namespace
        {
            wchar_t calloutName[]{ L"Flow V6 callout" };
            wchar_t calloutDescription[]{ L"Flow V6 callout description" };
        }

        constexpr FWPM_CALLOUT callout
        {
            .calloutKey{ 0x379f839e, 0x7d31, 0x4b20, { 0x9e, 0x8f, 0x8c, 0x5b, 0x7e, 0xf3, 0xf3, 0xb } },
            .displayData{.name{calloutName}, .description{calloutDescription}},
            .applicableLayer{ 0x7021d2b3, 0xdfa4, 0x406e, { 0xaf, 0xeb, 0x6a, 0xfa, 0xf7, 0xe7, 0x0e, 0xfd } } //FWPM_LAYER_ALE_FLOW_ESTABLISHED_V6
        };

        namespace
        {
            wchar_t sublayerName[]{ L"Flow V6 sublayer" };
            wchar_t sublayerDescription[]{ L"Flow V6 sublayer description" };
        }

        constexpr FWPM_SUBLAYER sublayer
        {
            .subLayerKey{ 0xf53ffe91, 0x6d87, 0x4dc0, { 0xab, 0xc6, 0xf5, 0x6c, 0x32, 0xbb, 0x53, 0x69 } },
            .displayData{.name{ sublayerName }, .description{ sublayerDescription } }
        };

        namespace
        {
            wchar_t filterName[]{ L"Flow V6 filter" };
            wchar_t filterDescription[]{ L"Flow V6 filter description" };
        }

        constexpr FWPM_FILTER filter
        {
            .filterKey{ 0x6e3ace4b, 0x75ea, 0x4093, { 0xba, 0x99, 0xff, 0x1c, 0x3e, 0xcf, 0xdf, 0xef } },
            .displayData{.name{filterName}, .description{filterDescription}},
            .layerKey{callout.applicableLayer},
            .subLayerKey{sublayer.subLayerKey},
            .action{.type{FWP_ACTION_CALLOUT_INSPECTION}, .calloutKey{callout.calloutKey}}
        };
    }

    constexpr FWPS_CALLOUT callout
    {
        .calloutKey{management::callout.calloutKey},
            .classifyFn{classify},
            .notifyFn{notify}
    };
}
