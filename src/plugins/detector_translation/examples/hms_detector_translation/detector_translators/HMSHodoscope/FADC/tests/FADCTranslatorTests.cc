#include <cassert>

#include <JANA/JEvent.h>
#include <JANA/JException.h>

#include "FADCTranslator.h"

int main() {
    const DetectorAddress address {
        "HMS_HODOSCOPE",
        {{"plane", 1}, {"bar", 2}, {"signal", 0}}
    };

    FADC250PulseHit pulse(12, 34, 56, 1, 3, 250, 0, 7, 80);
    pulse.integral_sum = 10430;
    pulse.integral_quality = 1;
    pulse.nsamples_above_th = 9;
    pulse.coarse_time = 10;
    pulse.fine_time = 11;
    pulse.pulse_peak = 120;
    pulse.time_quality = 2;
    const auto pulse_hit = makeHMSHodoscopeFADCPulseDigiHit(pulse, address);
    assert(pulse_hit.plane == 1);
    assert(pulse_hit.bar == 2);
    assert(pulse_hit.signal == 0);
    assert(pulse_hit.integral_sum == 10430);
    assert(pulse_hit.nsamples_above_threshold == 9);

    FADC250WaveformHit waveform(13, 35, 57, 1, 3, 250, 0);
    waveform.waveform = {101, 102, 103};
    const auto waveform_hit =
        makeHMSHodoscopeFADCWaveformDigiHit(waveform, address);
    assert(waveform_hit.plane == 1);
    assert(waveform_hit.waveform.size() == 3);
    assert(waveform_hit.waveform[2] == 103);

    const FADC250HallBPulseIntegralHit integral(
        14, 36, 58, 1, 3, 250, 0, 2, 900);
    const auto integral_hit =
        makeHMSHodoscopeFADCPulseIntegralDigiHit(integral, address);
    assert(integral_hit.bar == 2);
    assert(integral_hit.pulse_number == 2);
    assert(integral_hit.pulse_integral == 900);

    const FADC250HallBPulseTimeHit time(
        15, 37, 59, 1, 3, 250, 0, 3, 1, 44, 5);
    const auto time_hit =
        makeHMSHodoscopeFADCPulseTimeDigiHit(time, address);
    assert(time_hit.signal == 0);
    assert(time_hit.measurement_quality_factor == 1);
    assert(time_hit.coarse_pulse_time == 44);
    assert(time_hit.fine_pulse_time == 5);

    const FADC250HallBPulsePeakHit peak(
        16, 38, 60, 1, 3, 250, 0, 1, 20, 450);
    const auto peak_hit =
        makeHMSHodoscopeFADCPulsePeakDigiHit(peak, address);
    assert(peak_hit.plane == 1);
    assert(peak_hit.minimum_voltage == 20);
    assert(peak_hit.peak_voltage == 450);

    JEvent event;
    translateHMSHodoscopeFADCPulseHit(pulse, address, event);
    translateHMSHodoscopeFADCWaveformHit(waveform, address, event);
    translateHMSHodoscopeFADCPulseIntegralHit(integral, address, event);
    translateHMSHodoscopeFADCPulseTimeHit(time, address, event);
    translateHMSHodoscopeFADCPulsePeakHit(peak, address, event);
    assert(event.Get<HMSHodoscopeFADCPulseDigiHit>().size() == 1);
    assert(event.Get<HMSHodoscopeFADCWaveformDigiHit>().size() == 1);
    assert(event.Get<HMSHodoscopeFADCPulseIntegralDigiHit>().size() == 1);
    assert(event.Get<HMSHodoscopeFADCPulseTimeDigiHit>().size() == 1);
    assert(event.Get<HMSHodoscopeFADCPulsePeakDigiHit>().size() == 1);

    try {
        makeHMSHodoscopeFADCPulseDigiHit(
            pulse,
            {"OTHER_DETECTOR", {{"plane", 1}, {"bar", 2}, {"signal", 0}}});
        assert(false);
    }
    catch (const JException&) {
    }

    try {
        makeHMSHodoscopeFADCPulseDigiHit(
            pulse,
            {"HMS_HODOSCOPE", {{"plane", 1}, {"bar", 2}}});
        assert(false);
    }
    catch (const JException&) {
    }
}
