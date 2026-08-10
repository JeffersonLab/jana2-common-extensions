#include "DetectorDigiHit_factory.h"

#include <JANA/JEvent.h>
#include <JANA/JException.h>

#include <cstdint>
#include <string>

namespace {

std::int32_t RequireField(const DetectorAddress& address, const std::string& name) {
    for (const auto& [field_name, value] : address.fields) {
        if (field_name == name) {
            return value;
        }
    }
    throw JException(
        "HMS_HODOSCOPE detector address is missing required field '%s'",
        name.c_str());
}

} // namespace

HMSHodoscopeDigiHit MakeHMSHodoscopeDigiHit(
    const FADC250PulseHit& pulse,
    const DetectorAddress& address) {
    if (address.detector != "HMS_HODOSCOPE") {
        throw JException(
            "Cannot create HMSHodoscopeDigiHit from detector '%s'",
            address.detector.c_str());
    }

    return {
        RequireField(address, "plane"),
        RequireField(address, "bar"),
        RequireField(address, "signal"),
        pulse.trigger_num,
        pulse.timestamp1,
        pulse.timestamp2,
        pulse.rocid,
        pulse.slot,
        pulse.module_id,
        pulse.chan,
        pulse.pedestal_quality,
        pulse.pedestal_sum,
        pulse.integral_sum,
        pulse.integral_quality,
        pulse.nsamples_above_th,
        pulse.coarse_time,
        pulse.fine_time,
        pulse.pulse_peak,
        pulse.time_quality
    };
}

DetectorDigiHit_factory::DetectorDigiHit_factory() {
    SetTypeName("DetectorDigiHit_factory");
    SetPrefix("detector_digi_hits");
}

void DetectorDigiHit_factory::ChangeRun(const JEvent& event) {
    m_translation_table =
        m_translation_table_service->GetTable(event.GetRunNumber());
}

void DetectorDigiHit_factory::Process(const JEvent&) {
    const auto& fadc_pulses = m_fadc_pulses_in();
    auto& hms_hodoscope_hits = m_hms_hodoscope_hits_out();
    hms_hodoscope_hits.reserve(fadc_pulses.size());

    for (const auto* pulse : fadc_pulses) {
        const auto* address = m_translation_table->Lookup({
            pulse->rocid,
            pulse->slot,
            pulse->chan
        });
        if (address == nullptr || address->detector != "HMS_HODOSCOPE") {
            continue;
        }

        hms_hodoscope_hits.push_back(
            new HMSHodoscopeDigiHit(MakeHMSHodoscopeDigiHit(*pulse, *address)));
    }
}
