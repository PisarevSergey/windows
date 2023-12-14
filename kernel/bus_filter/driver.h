#pragma once

class Driver final {
public:
    Driver(Driver&&) = delete;
private:
};

extern Driver& g_driver;