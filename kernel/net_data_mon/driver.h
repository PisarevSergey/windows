#pragma once

class Driver final
{
public:
    Driver() = default;

    [[nodiscard]] NTSTATUS Init(DRIVER_OBJECT& driveObject);

    Driver(Driver&&) = delete;

    void* operator new(size_t, void* p) { return p; }
    void operator delete(void*) {}
private:
    Tracer m_tracer;
    flt::AutoMinifilter m_minifilter;
    nt::AutoDevice m_wfpDevice;
    Callouts m_callouts;
};

extern Driver& g_driver;
