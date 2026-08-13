#include "JEventProcessor_DetectorTranslationDump.h"

#include <JANA/JEvent.h>
#include <JANA/JException.h>

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <system_error>

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

std::ofstream openOutput(
    const std::filesystem::path& directory,
    const std::string& filename,
    const std::string& header) {
    const auto path = directory / filename;
    std::ofstream output(path);
    if (!output.is_open()) {
        throw JException(
            "Failed to open detector translation dump output: %s",
            path.string().c_str());
    }
    output << header << '\n';
    return output;
}

template <typename HitT>
void writeFADCFields(
    std::ofstream& output,
    std::uint64_t event_number,
    const HitT& hit) {
    output
        << event_number << ','
        << hit.rocid << ','
        << hit.slot << ','
        << hit.module_id << ','
        << hit.channel << ','
        << hit.plane << ','
        << hit.bar << ','
        << hit.signal << ','
        << hit.trigger_num << ','
        << hit.timestamp1 << ','
        << hit.timestamp2;
}

} // namespace

JEventProcessor_DetectorTranslationDump::
JEventProcessor_DetectorTranslationDump() {
    SetTypeName("JEventProcessor_DetectorTranslationDump");
    SetPrefix("detector_translation_dump");
    SetCallbackStyle(CallbackStyle::ExpertMode);
    m_fadcPulses.SetOptional(true);
    m_fadcWaveforms.SetOptional(true);
    m_fadcPulseIntegrals.SetOptional(true);
    m_fadcPulseTimes.SetOptional(true);
    m_fadcPulsePeaks.SetOptional(true);
    m_fadcScalers.SetOptional(true);
}

void JEventProcessor_DetectorTranslationDump::Init() {
    const std::filesystem::path directory(m_outputDirectory());
    std::error_code error;
    std::filesystem::create_directories(directory, error);
    if (error) {
        throw JException(
            "Failed to create detector translation dump directory '%s': %s",
            directory.string().c_str(),
            error.message().c_str());
    }

    const std::string fadcFields =
        "event,rocid,slot,module_id,channel,plane,bar,signal,trigger_num,"
        "timestamp1,timestamp2";
    m_fadcPulseOutput = openOutput(
        directory,
        "hms_fadc_pulse.csv",
        fadcFields +
            ",pedestal_quality,pedestal_sum,integral_sum,integral_quality,"
            "nsamples_above_threshold,coarse_time,fine_time,pulse_peak,"
            "time_quality");
    m_fadcWaveformOutput = openOutput(
        directory,
        "hms_fadc_waveform.csv",
        fadcFields + ",waveform");
    m_fadcPulseIntegralOutput = openOutput(
        directory,
        "hms_fadc_pulse_integral.csv",
        fadcFields + ",pulse_number,pulse_integral");
    m_fadcPulseTimeOutput = openOutput(
        directory,
        "hms_fadc_pulse_time.csv",
        fadcFields +
            ",pulse_number,measurement_quality_factor,coarse_pulse_time,"
            "fine_pulse_time");
    m_fadcPulsePeakOutput = openOutput(
        directory,
        "hms_fadc_pulse_peak.csv",
        fadcFields + ",pulse_number,minimum_voltage,peak_voltage");
    m_fadcScalerOutput = openOutput(
        directory,
        "hms_fadc_scaler.csv",
        "event,rocid,slot,plane,bar,signal,words_index,number_of_counts,counts");
}

void JEventProcessor_DetectorTranslationDump::ProcessSequential(
    const JEvent& event) {
    const auto event_number = event.GetEventNumber();

    for (const auto* hit : m_fadcPulses()) {
        writeFADCFields(m_fadcPulseOutput, event_number, *hit);
        m_fadcPulseOutput
            << ',' << hit->pedestal_quality
            << ',' << hit->pedestal_sum
            << ',' << hit->integral_sum
            << ',' << hit->integral_quality
            << ',' << hit->nsamples_above_threshold
            << ',' << hit->coarse_time
            << ',' << hit->fine_time
            << ',' << hit->pulse_peak
            << ',' << hit->time_quality << '\n';
    }
    for (const auto* hit : m_fadcWaveforms()) {
        writeFADCFields(m_fadcWaveformOutput, event_number, *hit);
        m_fadcWaveformOutput << ',' << join(hit->waveform) << '\n';
    }
    for (const auto* hit : m_fadcPulseIntegrals()) {
        writeFADCFields(m_fadcPulseIntegralOutput, event_number, *hit);
        m_fadcPulseIntegralOutput
            << ',' << hit->pulse_number
            << ',' << hit->pulse_integral << '\n';
    }
    for (const auto* hit : m_fadcPulseTimes()) {
        writeFADCFields(m_fadcPulseTimeOutput, event_number, *hit);
        m_fadcPulseTimeOutput
            << ',' << hit->pulse_number
            << ',' << hit->measurement_quality_factor
            << ',' << hit->coarse_pulse_time
            << ',' << hit->fine_pulse_time << '\n';
    }
    for (const auto* hit : m_fadcPulsePeaks()) {
        writeFADCFields(m_fadcPulsePeakOutput, event_number, *hit);
        m_fadcPulsePeakOutput
            << ',' << hit->pulse_number
            << ',' << hit->minimum_voltage
            << ',' << hit->peak_voltage << '\n';
    }
    for (const auto* hit : m_fadcScalers()) {
        m_fadcScalerOutput
            << event_number << ','
            << hit->rocid << ','
            << hit->slot << ','
            << hit->plane << ','
            << hit->bar << ','
            << hit->signal << ','
            << hit->words_index << ','
            << hit->number_of_counts << ','
            << join(hit->counts) << '\n';
    }
}

void JEventProcessor_DetectorTranslationDump::Finish() {
    m_fadcPulseOutput.close();
    m_fadcWaveformOutput.close();
    m_fadcPulseIntegralOutput.close();
    m_fadcPulseTimeOutput.close();
    m_fadcPulsePeakOutput.close();
    m_fadcScalerOutput.close();
}
