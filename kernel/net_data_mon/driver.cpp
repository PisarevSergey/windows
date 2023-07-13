#include "pch.h"

TRACELOGGING_DEFINE_PROVIDER(g_tracer, "NetDataMonLoggingProviderKM", (0xa087757e, 0xa1d9, 0x4261, 0x97, 0x1c, 0xe4, 0x17, 0x3c, 0x4b, 0x7d, 0x1e));

namespace
{
    alignas(Driver) char driverMemory[sizeof(Driver)];
}

Driver& g_driver = *reinterpret_cast<Driver*>(driverMemory);

extern "C" NTSTATUS DriverEntry([[maybe_unused]] PDRIVER_OBJECT driverObject, [[maybe_unused]] PUNICODE_STRING regPath)
{
    new(&g_driver) Driver;
    g_driver.~Driver();

    return STATUS_UNSUCCESSFUL;
}
