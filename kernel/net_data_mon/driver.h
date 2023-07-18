#pragma once

class ClientCommunicationPort final
{
public:
    ClientCommunicationPort(flt::AutoMinifilter& filter) : m_filter{ filter } {}

    NTSTATUS OnConnect(PFLT_PORT clientPort);

    void OnDisconnect();

    NTSTATUS OnMessageNotify(IN PVOID InputBuffer OPTIONAL,
        IN ULONG InputBufferLength,
        OUT PVOID OutputBuffer OPTIONAL,
        IN ULONG OutputBufferLength,
        OUT ULONG& ReturnOutputBufferLength);

    NTSTATUS SendMessage(void* senderBuffer,
        ULONG senderBufferSize,
        PLARGE_INTEGER timeout = nullptr,
        void* replyBuffer = nullptr,
        PULONG replyBufferSize = nullptr);

    ClientCommunicationPort(ClientCommunicationPort&&) = delete;
private:
    flt::AutoMinifilter& m_filter;
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
    ClientCommunicationPort m_clientCommunicationPort{m_minifilter};
    nt::AutoDevice m_wfpDevice;
    Callouts m_callouts;
    AutoBfe m_bfe;
};

extern Driver& g_driver;
