#pragma once

#include <compare>
#include <cstdint>
#include <limits>

struct DAQAddress {
    static constexpr std::uint32_t UnspecifiedChannel =
        std::numeric_limits<std::uint32_t>::max();

    std::uint32_t rocid;
    std::uint32_t slot;
    std::uint32_t channel;

    auto operator<=>(const DAQAddress&) const = default;
};
