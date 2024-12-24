#pragma once

class Driver final {
public:
    Driver(PDRIVER_OBJECT, PUNICODE_STRING);
    ~Driver() = default;

    Driver(Driver&&) = delete;

    class Wpp final {
    public:
        Wpp(DRIVER_OBJECT& driver_object, UNICODE_STRING& reg_path);
        ~Wpp();

        Wpp(Wpp&&) = delete;
    private:
        DRIVER_OBJECT& m_driver;
    };

private:
    Wpp m_wpp;
};

inline alignas(Driver) char driver_memory[sizeof(Driver)];
inline Driver& g_driver = *reinterpret_cast<Driver*>(driver_memory);