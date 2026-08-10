#include <cassert>

#include <JANA/JEvent.h>

#include "HMSHodoscopeTranslator.h"

int main() {
    FADC250PulseHit pulse(12, 34, 56, 1, 3, 250, 0, 7, 80);
    pulse.integral_sum = 10430;
    pulse.integral_quality = 1;
    pulse.nsamples_above_th = 9;
    pulse.coarse_time = 10;
    pulse.fine_time = 11;
    pulse.pulse_peak = 120;
    pulse.time_quality = 2;

    const DetectorAddress address {
        "HMS_HODOSCOPE",
        {{"plane", 1}, {"bar", 2}, {"signal", 0}}
    };

    const auto hit = makeHMSHodoscopeDigiHit(pulse, address);
    assert(hit.plane == 1);
    assert(hit.bar == 2);
    assert(hit.signal == 0);
    assert(hit.trigger_num == 12);
    assert(hit.timestamp1 == 34);
    assert(hit.timestamp2 == 56);
    assert(hit.rocid == 1);
    assert(hit.slot == 3);
    assert(hit.module_id == 250);
    assert(hit.channel == 0);
    assert(hit.pedestal_quality == 7);
    assert(hit.pedestal_sum == 80);
    assert(hit.integral_sum == 10430);
    assert(hit.integral_quality == 1);
    assert(hit.nsamples_above_threshold == 9);
    assert(hit.coarse_time == 10);
    assert(hit.fine_time == 11);
    assert(hit.pulse_peak == 120);
    assert(hit.time_quality == 2);

    JEvent event;
    translateHMSHodoscopeFADCHit(pulse, address, event);
    const auto inserted_hits = event.Get<HMSHodoscopeDigiHit>();
    assert(inserted_hits.size() == 1);
    assert(inserted_hits.front()->integral_sum == 10430);
}
