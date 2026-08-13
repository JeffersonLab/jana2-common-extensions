#include "FADCTranslator.h"

#include <JANA/JEvent.h>

#include "HMSHodoscopeIdentity.h"

HMSHodoscopeFADCDigiHit makeHMSHodoscopeFADCDigiHit(
    const FADC250PulseHit& pulse,
    const DetectorAddress& address) {
    const auto detector = getHMSHodoscopeIdentity(address);

    return {
        detector.plane,
        detector.bar,
        detector.signal,
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

void translateHMSHodoscopeFADCHit(
    const FADC250PulseHit& pulse,
    const DetectorAddress& address,
    const JEvent& event) {
    event.Insert(new HMSHodoscopeFADCDigiHit(
        makeHMSHodoscopeFADCDigiHit(pulse, address)));
}
