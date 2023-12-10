#pragma once

class Driver final
{
public:
    Driver() = default;
    ~Driver() = default;

    void Insert(UsbFilterDeviceContext& context);
    void Remove(UsbFilterDeviceContext& context);
    void StartFilterUsbDevice(const UNICODE_STRING& deviceId);

    Driver(Driver&&) = delete;

    void* operator new(size_t, void* p) { return p; }
    void operator delete(void*, size_t) {}
private:
    Tracer m_tracer;
    UsbFilters m_usbFilters;
};

extern Driver& g_driver;
