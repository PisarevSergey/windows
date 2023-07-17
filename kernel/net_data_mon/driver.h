#pragma once

class ClientCommunicationPort final
{
public:
    NTSTATUS OnConnect(PFLT_PORT clientPort,
        void* serverPortCookie,
        void** connectionPortCookie);

    void OnDisconnect();

    NTSTATUS OnMessageNotify(IN PVOID InputBuffer OPTIONAL,
        IN ULONG InputBufferLength,
        OUT PVOID OutputBuffer OPTIONAL,
        IN ULONG OutputBufferLength,
        OUT PULONG ReturnOutputBufferLength);

private:
    PFLT_PORT m_port{};
};

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
    flt::AutoServerPort m_serverCommunicationPort;
    ClientCommunicationPort m_clientCommunicationPort;
    nt::AutoDevice m_wfpDevice;
    Callouts m_callouts;
};

extern Driver& g_driver;
