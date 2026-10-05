#include "FADCDumpWriter.h"

#include <sstream>

namespace {

template <typename RangeT>
std::string join(const RangeT& values) {
    std::ostringstream output;
    for (std::size_t index = 0; index < values.size(); ++index) {
        if (index != 0) {
            output << '|';
        }
        output << values[index];
    }
    return output.str();
}

template <typename HitT>
void writeFADCFields(
    std::ostream& output,
    std::uint64_t event_number,
    const HitT& hit) {
    output
        << event_number << ','
        << hit.rocid << ','
        << hit.slot << ','
        << hit.channel << ','
        << hit.module_id << ','
        << hit.plane << ','
        << hit.bar << ','
        << hit.signal << ','
        << hit.trigger_num << ','
        << hit.timestamp1 << ','
        << hit.timestamp2;
}

} // namespace

const std::string HMSHodoscopeFADCPulseCSVHeader = "event,rocid,slot,channel,module_id,plane,bar,signal,trigger_num,timestamp1,timestamp2,pedestal_quality,pedestal_sum,integral_sum,integral_quality,nsamples_above_threshold,coarse_time,fine_time,pulse_peak,time_quality";

void writeHMSHodoscopeFADCPulseCSVRow(
    std::ostream& output, std::uint64_t event_number,
    const HMSHodoscopeFADCPulseDigiHit& hit) {
    writeFADCFields(output, event_number, hit);
    output
        << ',' << hit.pedestal_quality
        << ',' << hit.pedestal_sum
        << ',' << hit.integral_sum
        << ',' << hit.integral_quality
        << ',' << hit.nsamples_above_threshold
        << ',' << hit.coarse_time
        << ',' << hit.fine_time
        << ',' << hit.pulse_peak
        << ',' << hit.time_quality << '\n';
}

const std::string HMSHodoscopeFADCWaveformCSVHeader = "event,rocid,slot,channel,module_id,plane,bar,signal,trigger_num,timestamp1,timestamp2,waveform";

void writeHMSHodoscopeFADCWaveformCSVRow(
    std::ostream& output, std::uint64_t event_number,
    const HMSHodoscopeFADCWaveformDigiHit& hit) {
    writeFADCFields(output, event_number, hit);
    output << ',' << join(hit.waveform) << '\n';
}

const std::string HMSHodoscopeFADCPulseIntegralCSVHeader = "event,rocid,slot,channel,module_id,plane,bar,signal,trigger_num,timestamp1,timestamp2,pulse_number,pulse_integral";

void writeHMSHodoscopeFADCPulseIntegralCSVRow(
    std::ostream& output, std::uint64_t event_number,
    const HMSHodoscopeFADCPulseIntegralDigiHit& hit) {
    writeFADCFields(output, event_number, hit);
    output
        << ',' << hit.pulse_number
        << ',' << hit.pulse_integral << '\n';
}

const std::string HMSHodoscopeFADCPulseTimeCSVHeader = "event,rocid,slot,channel,module_id,plane,bar,signal,trigger_num,timestamp1,timestamp2,pulse_number,measurement_quality_factor,coarse_pulse_time,fine_pulse_time";

void writeHMSHodoscopeFADCPulseTimeCSVRow(
    std::ostream& output, std::uint64_t event_number,
    const HMSHodoscopeFADCPulseTimeDigiHit& hit) {
    writeFADCFields(output, event_number, hit);
    output
        << ',' << hit.pulse_number
        << ',' << hit.measurement_quality_factor
        << ',' << hit.coarse_pulse_time
        << ',' << hit.fine_pulse_time << '\n';
}

const std::string HMSHodoscopeFADCPulsePeakCSVHeader = "event,rocid,slot,channel,module_id,plane,bar,signal,trigger_num,timestamp1,timestamp2,pulse_number,minimum_voltage,peak_voltage";

void writeHMSHodoscopeFADCPulsePeakCSVRow(
    std::ostream& output, std::uint64_t event_number,
    const HMSHodoscopeFADCPulsePeakDigiHit& hit) {
    writeFADCFields(output, event_number, hit);
    output
        << ',' << hit.pulse_number
        << ',' << hit.minimum_voltage
        << ',' << hit.peak_voltage << '\n';
}
