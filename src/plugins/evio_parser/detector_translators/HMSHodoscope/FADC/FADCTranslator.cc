#include "FADCTranslator.h"

#include <JANA/JEvent.h>

#include "HMSHodoscopeIdentity.h"

HMSHodoscopeFADCPulseDigiHit makeHMSHodoscopeFADCPulseDigiHit(
    const FADC250PulseHit& pulse,
    const DetectorAddress& address) {
    const auto detector = getHMSHodoscopeIdentity(address);
    return {
        detector.plane, detector.bar, detector.signal,
        pulse.trigger_num, pulse.timestamp1, pulse.timestamp2,
        pulse.rocid, pulse.slot, pulse.module_id, pulse.chan,
        pulse.pedestal_quality, pulse.pedestal_sum, pulse.integral_sum,
        pulse.integral_quality, pulse.nsamples_above_th, pulse.coarse_time,
        pulse.fine_time, pulse.pulse_peak, pulse.time_quality
    };
}

HMSHodoscopeFADCWaveformDigiHit makeHMSHodoscopeFADCWaveformDigiHit(
    const FADC250WaveformHit& waveform,
    const DetectorAddress& address) {
    const auto detector = getHMSHodoscopeIdentity(address);
    return {
        detector.plane, detector.bar, detector.signal,
        waveform.trigger_num, waveform.timestamp1, waveform.timestamp2,
        waveform.rocid, waveform.slot, waveform.module_id, waveform.chan,
        waveform.waveform
    };
}

HMSHodoscopeFADCPulseIntegralDigiHit
makeHMSHodoscopeFADCPulseIntegralDigiHit(
    const FADC250HallBPulseIntegralHit& integral,
    const DetectorAddress& address) {
    const auto detector = getHMSHodoscopeIdentity(address);
    return {
        detector.plane, detector.bar, detector.signal,
        integral.trigger_num, integral.timestamp1, integral.timestamp2,
        integral.rocid, integral.slot, integral.module_id, integral.chan,
        integral.pulse_number, integral.pulse_integral
    };
}

HMSHodoscopeFADCPulseTimeDigiHit makeHMSHodoscopeFADCPulseTimeDigiHit(
    const FADC250HallBPulseTimeHit& time,
    const DetectorAddress& address) {
    const auto detector = getHMSHodoscopeIdentity(address);
    return {
        detector.plane, detector.bar, detector.signal,
        time.trigger_num, time.timestamp1, time.timestamp2,
        time.rocid, time.slot, time.module_id, time.chan,
        time.pulse_number, time.measurement_quality_factor,
        time.coarse_pulse_time, time.fine_pulse_time
    };
}

HMSHodoscopeFADCPulsePeakDigiHit makeHMSHodoscopeFADCPulsePeakDigiHit(
    const FADC250HallBPulsePeakHit& peak,
    const DetectorAddress& address) {
    const auto detector = getHMSHodoscopeIdentity(address);
    return {
        detector.plane, detector.bar, detector.signal,
        peak.trigger_num, peak.timestamp1, peak.timestamp2,
        peak.rocid, peak.slot, peak.module_id, peak.chan,
        peak.pulse_number, peak.Vmin, peak.Vpeak
    };
}

void translateHMSHodoscopeFADCPulseHit(
    const FADC250PulseHit& pulse,
    const DetectorAddress& address,
    const JEvent& event) {
    event.Insert(new HMSHodoscopeFADCPulseDigiHit(
        makeHMSHodoscopeFADCPulseDigiHit(pulse, address)));
}

void translateHMSHodoscopeFADCWaveformHit(
    const FADC250WaveformHit& waveform,
    const DetectorAddress& address,
    const JEvent& event) {
    event.Insert(new HMSHodoscopeFADCWaveformDigiHit(
        makeHMSHodoscopeFADCWaveformDigiHit(waveform, address)));
}

void translateHMSHodoscopeFADCPulseIntegralHit(
    const FADC250HallBPulseIntegralHit& integral,
    const DetectorAddress& address,
    const JEvent& event) {
    event.Insert(new HMSHodoscopeFADCPulseIntegralDigiHit(
        makeHMSHodoscopeFADCPulseIntegralDigiHit(integral, address)));
}

void translateHMSHodoscopeFADCPulseTimeHit(
    const FADC250HallBPulseTimeHit& time,
    const DetectorAddress& address,
    const JEvent& event) {
    event.Insert(new HMSHodoscopeFADCPulseTimeDigiHit(
        makeHMSHodoscopeFADCPulseTimeDigiHit(time, address)));
}

void translateHMSHodoscopeFADCPulsePeakHit(
    const FADC250HallBPulsePeakHit& peak,
    const DetectorAddress& address,
    const JEvent& event) {
    event.Insert(new HMSHodoscopeFADCPulsePeakDigiHit(
        makeHMSHodoscopeFADCPulsePeakDigiHit(peak, address)));
}
