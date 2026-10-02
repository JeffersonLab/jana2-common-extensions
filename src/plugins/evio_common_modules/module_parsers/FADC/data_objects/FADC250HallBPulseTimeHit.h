#ifndef FADC250HALLBPULSETIMEHIT_H
#define FADC250HALLBPULSETIMEHIT_H

#include "FADC250Hit.h"

/**
 * @class FADC250HallBPulseTimeHit
 * @brief FADC250 HallB-mode pulse time hit data
 */
class FADC250HallBPulseTimeHit : public FADC250Hit {
public:
    uint32_t pulse_number;
    uint32_t measurement_quality_factor;
    uint32_t coarse_pulse_time;
    uint32_t fine_pulse_time;

    /**
     * @brief Default constructor
     * 
     * Initializes all members to zero.
     */
    FADC250HallBPulseTimeHit() : FADC250Hit(),
        pulse_number(0), measurement_quality_factor(0),
        coarse_pulse_time(0), fine_pulse_time(0) {}

    /**
     * @brief Parameterized constructor
     * 
     * Initializes the base hit information and pulse time data.
     * 
     * @param trigger_num Trigger number for this hit
     * @param timestamp1 First timestamp value
     * @param timestamp2 Second timestamp value
     * @param rocid Readout Controller ID
     * @param slot Slot number in the crate
     * @param module_id Module identifier
     * @param chan Channel number
     * @param pulse_number Pulse number within the event
     * @param measurement_quality_factor Quality factor for the measurement
     * @param coarse_pulse_time Coarse pulse time value
     * @param fine_pulse_time Fine pulse time value
     */
    FADC250HallBPulseTimeHit(uint64_t trigger_num, uint64_t timestamp1, uint64_t timestamp2,
             uint32_t rocid, uint32_t slot, uint32_t module_id, uint32_t chan,
             uint32_t pulse_number, uint32_t measurement_quality_factor,
             uint32_t coarse_pulse_time, uint32_t fine_pulse_time)
        : FADC250Hit(trigger_num, timestamp1, timestamp2, rocid, slot, module_id, chan),
          pulse_number(pulse_number), measurement_quality_factor(measurement_quality_factor),
          coarse_pulse_time(coarse_pulse_time), fine_pulse_time(fine_pulse_time) {}
};

#endif // FADC250HALLBPULSETIMEHIT_H
