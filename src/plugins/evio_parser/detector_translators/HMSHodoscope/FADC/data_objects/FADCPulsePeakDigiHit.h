#pragma once

#include <cstdint>

struct HMSHodoscopeFADCPulsePeakDigiHit {
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

    std::uint32_t pulse_number;
    std::uint32_t minimum_voltage;
    std::uint32_t peak_voltage;
};
