#pragma once

#include <cstdint>

struct HMSHodoscopeDigiHit {
    std::int32_t plane;
    std::int32_t bar;
    std::int32_t signal;

    std::uint64_t trigger_num;
    std::uint32_t timestamp1;
    std::uint32_t timestamp2;
    std::uint32_t rocid;
    std::uint32_t slot;
    std::uint32_t module_id;
    std::uint32_t channel;

    std::uint32_t pedestal_quality;
    std::uint32_t pedestal_sum;
    std::uint32_t integral_sum;
    std::uint32_t integral_quality;
    std::uint32_t nsamples_above_threshold;
    std::uint32_t coarse_time;
    std::uint32_t fine_time;
    std::uint32_t pulse_peak;
    std::uint32_t time_quality;
};
