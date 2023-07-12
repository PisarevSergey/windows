#include "pch.h"

extern "C" NTSTATUS DriverEntry([[maybe_unused]] PDRIVER_OBJECT driverObject, [[maybe_unused]] PUNICODE_STRING regPath)
{
    return STATUS_UNSUCCESSFUL;
}
