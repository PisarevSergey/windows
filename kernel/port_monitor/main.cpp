#include "pch.h"

#include "main.tmh"

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING reg_path)
{
    new(&g_driver) Driver{ *driver, *reg_path };

    info(MAIN, "driver entry");

    g_driver.~Driver();

    return STATUS_UNSUCCESSFUL;
}
