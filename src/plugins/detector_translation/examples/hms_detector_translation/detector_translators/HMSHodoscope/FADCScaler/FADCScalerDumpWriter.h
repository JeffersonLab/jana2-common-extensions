#pragma once

#include <cstdint>
#include <ostream>
#include <string>

#include "FADCScalerDigiHit.h"

extern const std::string HMSHodoscopeFADCScalerCSVHeader;
void writeHMSHodoscopeFADCScalerCSVRow(
    std::ostream& output, std::uint64_t event_number,
    const HMSHodoscopeFADCScalerDigiHit& hit);
