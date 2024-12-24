#include "pch.h"

#include <kcpp/scope_guard.h>

#include "driver.tmh"

static void driver_unload([[maybe_unused]] WDFDRIVER driver) {
    TraceInfo(DRIVER, "unloading");
}

static void driver_context_cleanup([[maybe_unused]] WDFOBJECT driver) {
    KCPP_SCOPE_GUARD([] {g_driver.~Driver(); });
    TraceInfo(DRIVER, "driver cleanup");
}

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT driver_object, PUNICODE_STRING reg_path)
{
    new (&g_driver) Driver{ driver_object, reg_path };
    auto driver_releaser = kcpp::make_scope_guard([] {g_driver.~Driver(); });

    auto driver_object_attributes = kmdf::CreateObjectAttributes(nullptr, driver_context_cleanup);

    auto driver_config = kmdf::CreateDriverConfig(driver_unload);
    driver_config.DriverInitFlags |= WdfDriverInitNonPnpDriver;

    const auto status = WdfDriverCreate(driver_object, reg_path, &driver_object_attributes, &driver_config, nullptr);
    if (status != STATUS_SUCCESS)
    {
        TraceError(DRIVER, "WdfDriverCreate failed with %!STATUS!", status);
        return status;
    }
    driver_releaser.release();

    return STATUS_SUCCESS;
}

Driver::Wpp::Wpp(DRIVER_OBJECT& driver_object, UNICODE_STRING& reg_path) : m_driver{driver_object}
{
    WPP_INIT_TRACING(&m_driver, &reg_path);
}

Driver::Wpp::~Wpp()
{
    WPP_CLEANUP(&m_driver);
}

void __cdecl operator delete(void*, size_t) {
    ASSERT(FALSE);
}

Driver::Driver(PDRIVER_OBJECT driver_object, PUNICODE_STRING reg_path) : m_wpp{*driver_object, *reg_path} {
}
