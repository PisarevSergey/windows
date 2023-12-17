#pragma once

#define CONCAT2(a, b) a ## b
#define CONCAT(a, b) CONCAT2(a, b)

#define KCPP_UNUSED(expression) \
    [[maybe_unused]] auto&& CONCAT(unused, __COUNTER__) = expression
