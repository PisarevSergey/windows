#pragma once

class Driver final
{
public:
    Driver(DRIVER_OBJECT& driver, UNICODE_STRING& reg_path);
    ~Driver();

    Driver(Driver&&) = delete;

    void* operator new(size_t, void* p) { return p; }
private:
    DRIVER_OBJECT& m_driver;
};

extern Driver& g_driver;
