#pragma once

class Driver final
{
public:
    Driver() = default;
    ~Driver() = default;

    Driver(Driver&&) = delete;

    void* operator new(size_t, void* p) { return p; }
    void operator delete(void*, size_t) {}
private:
    Tracer m_tracer;
};

extern Driver& g_driver;
