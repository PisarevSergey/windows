#pragma once

namespace flow
{
    namespace v4
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
                wchar_t calloutName[]{ L"Flow V4 callout" };
                wchar_t calloutDescription[]{ L"Flow V4 callout description" };
            }

            constexpr FWPM_CALLOUT callout
            {
                .calloutKey{ 0xe52fb54b, 0xd2d4, 0x47fa, { 0x96, 0xd5, 0x24, 0x1c, 0xb5, 0xa0, 0x4, 0x4 } },
                .displayData{.name{calloutName}, .description{calloutDescription}},
                .applicableLayer{ 0xaf80470a, 0x5596, 0x4c13, { 0x99, 0x92, 0x53, 0x9e, 0x6f, 0xe5, 0x79, 0x67 } } // FWPM_LAYER_ALE_FLOW_ESTABLISHED_V4
            };

            namespace
            {
                wchar_t sublayerName[]{ L"Flow V4 sublayer" };
                wchar_t sublayerDescription[]{ L"Flow V4 sublayer description" };
            }

            constexpr FWPM_SUBLAYER sublayer
            {
                .subLayerKey{ 0xac50e04e, 0xcada, 0x4d23, { 0x8d, 0xd5, 0x2d, 0xab, 0x6b, 0xea, 0xf3, 0x8 } },
                .displayData{.name{ sublayerName }, .description{ sublayerDescription } }
            };

            namespace
            {
                wchar_t filterName[]{ L"Flow V4 filter" };
                wchar_t filterDescription[]{ L"Flow V4 filter description" };
            }

            constexpr FWPM_FILTER filter
            {
                .filterKey{ 0x41840695, 0xf026, 0x458c, { 0x8c, 0x7d, 0x1, 0x0, 0xe0, 0xbf, 0x50, 0xd0 } },
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

    namespace v6
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
}