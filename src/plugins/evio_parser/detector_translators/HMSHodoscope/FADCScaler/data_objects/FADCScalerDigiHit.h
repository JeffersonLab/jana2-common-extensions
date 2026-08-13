#pragma once

#include <array>
#include <cstdint>

struct HMSHodoscopeFADCScalerDigiHit {
    std::uint32_t rocid;
    std::uint32_t slot;
    std::uint32_t words_index;
    std::uint32_t number_of_counts;
    std::array<std::uint32_t, 16> counts;
};
