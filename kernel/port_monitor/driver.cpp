#include "pch.h"
#include "driver.tmh"

namespace {
    alignas(Driver) char driver_memory[sizeof(Driver)]{};
}

Driver& g_driver = *reinterpret_cast<Driver*>(driver_memory);

Driver::Driver(DRIVER_OBJECT& driver, UNICODE_STRING& reg_path) : m_driver{driver}
{
    WPP_INIT_TRACING(&m_driver, &reg_path);
}

Driver::~Driver()
{
    WPP_CLEANUP(&m_driver);
}
