#include "pch.h"

TRACELOGGING_DEFINE_PROVIDER(g_tracer, "BthMonLoggingProviderKM", (0x0776adb6, 0x9c11, 0x4cce, 0x82, 0xbd, 0x73, 0x43, 0x80, 0xf6, 0x75, 0x22));

namespace
{
    alignas(Driver) char driverMemory[sizeof(Driver)];
}

Driver& g_driver = *reinterpret_cast<Driver*>(driverMemory);

extern "C" NTSTATUS DriverEntry([[maybe_unused]] PDRIVER_OBJECT driverObject, [[maybe_unused]] PUNICODE_STRING regPath)
{
    new (&g_driver) Driver;
    TraceInfo("DriverEntry called");
    g_driver.~Driver();
    return STATUS_UNSUCCESSFUL;
}
