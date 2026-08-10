#include "HMSHodoscopeTranslator.h"

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
