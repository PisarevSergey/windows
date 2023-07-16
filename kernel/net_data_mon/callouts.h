#pragma once

struct WfpObjects final
{
    struct
    {
        FWPM_CALLOUT m_callout{};
        FWPM_SUBLAYER m_sublayer{};
        FWPM_FILTER m_filter{};
    } management;

    FWPS_CALLOUT m_callout{};
};

namespace f4 = flow::v4;
namespace f4m = f4::management;
constexpr WfpObjects flowV4objects{ .management{.m_callout{f4m::callout}, .m_sublayer{f4m::sublayer}, .m_filter{f4m::filter} }, .m_callout{f4::callout} };

namespace f6 = flow::v6;
namespace f6m = f6::management;
constexpr WfpObjects flowV6Objects{ .management{.m_callout{f6m::callout}, .m_sublayer{f6m::sublayer}, .m_filter{f6m::filter}}, .m_callout{f6::callout} };

constexpr WfpObjects wfpObjects[]
{
    flowV4objects,
    flowV6Objects
};

class Callouts final
{
public:
    [[nodiscard]]
    NTSTATUS Init(DEVICE_OBJECT& wfpDeviceObject);

    Callouts() = default;
    ~Callouts();

    Callouts(Callouts&&) = delete;
private:
    UINT32 m_registeredCalloutIds[ARRAYSIZE(wfpObjects)]{};
};
