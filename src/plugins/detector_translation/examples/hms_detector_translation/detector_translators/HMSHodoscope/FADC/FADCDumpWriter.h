#pragma once

#include <cstdint>
#include <ostream>
#include <string>

#include "FADCPulseDigiHit.h"
#include "FADCWaveformDigiHit.h"
#include "FADCPulseIntegralDigiHit.h"
#include "FADCPulseTimeDigiHit.h"
#include "FADCPulsePeakDigiHit.h"

extern const std::string HMSHodoscopeFADCPulseCSVHeader;
void writeHMSHodoscopeFADCPulseCSVRow(
    std::ostream& output, std::uint64_t event_number,
    const HMSHodoscopeFADCPulseDigiHit& hit);

extern const std::string HMSHodoscopeFADCWaveformCSVHeader;
void writeHMSHodoscopeFADCWaveformCSVRow(
    std::ostream& output, std::uint64_t event_number,
    const HMSHodoscopeFADCWaveformDigiHit& hit);

extern const std::string HMSHodoscopeFADCPulseIntegralCSVHeader;
void writeHMSHodoscopeFADCPulseIntegralCSVRow(
    std::ostream& output, std::uint64_t event_number,
    const HMSHodoscopeFADCPulseIntegralDigiHit& hit);

extern const std::string HMSHodoscopeFADCPulseTimeCSVHeader;
void writeHMSHodoscopeFADCPulseTimeCSVRow(
    std::ostream& output, std::uint64_t event_number,
    const HMSHodoscopeFADCPulseTimeDigiHit& hit);

extern const std::string HMSHodoscopeFADCPulsePeakCSVHeader;
void writeHMSHodoscopeFADCPulsePeakCSVRow(
    std::ostream& output, std::uint64_t event_number,
    const HMSHodoscopeFADCPulsePeakDigiHit& hit);
