#include "pch.h"

#include <kcpp/scope_guard.h>

#include "main.tmh"

void driver_unload([[maybe_unused]] WDFDRIVER driver) {
    info(MAIN, "unloading");
}

void driver_context_cleanup([[maybe_unused]] WDFOBJECT driver) {
    KCPP_SCOPE_GUARD([] {g_driver.~Driver(); });
    info(MAIN, "driver cleanup");
}

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING reg_path)
{
    new(&g_driver) Driver{ *driver, *reg_path };
    auto driver_guard = kcpp::make_scope_guard([] { g_driver.~Driver(); });

    auto driver_config = kmdf::CreateDriverConfig(driver_unload);
    driver_config.DriverInitFlags |= WdfDriverInitNonPnpDriver;

    auto driver_object_attributes = kmdf::CreateObjectAttributes(nullptr, driver_context_cleanup);

    const auto status = WdfDriverCreate(driver, reg_path, &driver_object_attributes, &driver_config, nullptr);
    if (!NT_SUCCESS(status))
    {
        error(MAIN, "WdfDriverCreate failed with %!STATUS!", status);
        return status;
    }
    driver_guard.release();

    info(MAIN, "driver load success");
    return STATUS_SUCCESS;
}
