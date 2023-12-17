#pragma once

class Driver final {
public:
    Driver(PDRIVER_OBJECT driverObject);

    NTSTATUS Attach(DEVICE_OBJECT& device);

    Driver(Driver&&) = delete;

    void* operator new(size_t, void* p) { return p; }
    void operator delete(void*, size_t) {}
private:
    Tracer m_tracer;
    DRIVER_OBJECT const * const m_driverObject;

    bool AreWeInStackFor(const DEVICE_OBJECT& device) const;
};

extern Driver& g_driver;