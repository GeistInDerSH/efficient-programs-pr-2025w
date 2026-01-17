#pragma once

#include <array>
#include <cstdint>

struct Board {
    std::array<std::uint8_t, 81> cells{};
};