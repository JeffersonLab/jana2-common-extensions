#ifndef _EVENT_HITS_FADC_H_
#define _EVENT_HITS_FADC_H_

#include <JANA/JEvent.h>

#include "EventHits.h"
#include "FADC250WaveformHit.h"
#include "FADC250PulseHit.h"
#include "FADC250HallBPulseIntegralHit.h"
#include "FADC250HallBPulseTimeHit.h"
#include "FADC250HallBPulsePeakHit.h"

/**
 * @class EventHits_FADC
 * @brief Container for all FADC detector hits in a single event
 *
 * Owns waveform and pulse hit pointers and knows how to insert
 * them into a JEvent.
 */
class EventHits_FADC : public EventHits {
public:
    std::vector<FADC250WaveformHit*> waveforms;
    std::vector<FADC250PulseHit*> pulses;
    std::vector<FADC250HallBPulseIntegralHit*> pulse_integrals;
    std::vector<FADC250HallBPulseTimeHit*> pulse_times;
    std::vector<FADC250HallBPulsePeakHit*> pulse_peaks;

    void insertIntoEvent(JEvent& event) override {
        for (auto& waveform : waveforms) {
            event.Insert(waveform);
        }
        for (auto& pulse : pulses) {
            event.Insert(pulse);
        }
        for (auto& pulse_integral : pulse_integrals) {
            event.Insert(pulse_integral);
        }
        for (auto& pulse_time : pulse_times) {
            event.Insert(pulse_time);
        }
        for (auto& pulse_peak : pulse_peaks) {
            event.Insert(pulse_peak);
        }
    }
};

#endif // _EVENT_HITS_FADC_H_
