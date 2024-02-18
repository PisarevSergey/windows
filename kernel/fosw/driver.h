#pragma once

class Driver final {
public:
    Driver() = default;

    [[nodiscard]]
    NTSTATUS Register(DRIVER_OBJECT& driverObject);

    [[nodiscard]]
    NTSTATUS Start();

    Driver(Driver&&) = delete;
    void* operator new(size_t, void* p) { return p; }
    void operator delete(void*, size_t) {}
private:
    Tracer m_tracer;
    flt::AutoMinifilter m_minifilter;
};

extern Driver& g_driver;
