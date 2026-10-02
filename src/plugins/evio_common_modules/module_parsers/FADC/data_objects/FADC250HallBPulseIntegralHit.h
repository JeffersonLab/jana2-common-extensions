#ifndef FADC250HALLBPULSEINTEGRALHIT_H
#define FADC250HALLBPULSEINTEGRALHIT_H

#include "FADC250Hit.h"

/**
 * @class FADC250HallBPulseIntegralHit
 * @brief FADC250 HallB-mode pulse integral hit data
 */
class FADC250HallBPulseIntegralHit : public FADC250Hit {
public:
    uint32_t pulse_number;
    uint32_t pulse_integral;

    /**
     * @brief Default constructor
     * 
     * Initializes all members to zero.
     */
    FADC250HallBPulseIntegralHit() : FADC250Hit(),
        pulse_number(0), pulse_integral(0) {}

    /**
     * @brief Parameterized constructor
     * 
     * Initializes the base hit information and pulse integral data.
     * 
     * @param trigger_num Trigger number for this hit
     * @param timestamp1 First timestamp value
     * @param timestamp2 Second timestamp value
     * @param rocid Readout Controller ID
     * @param slot Slot number in the crate
     * @param module_id Module identifier
     * @param chan Channel number
     * @param pulse_number Pulse number within the event
     * @param pulse_integral Pulse integral value
     */
    FADC250HallBPulseIntegralHit(uint64_t trigger_num, uint64_t timestamp1, uint64_t timestamp2,
             uint32_t rocid, uint32_t slot, uint32_t module_id, uint32_t chan,
             uint32_t pulse_number,uint32_t pulse_integral)
        : FADC250Hit(trigger_num, timestamp1, timestamp2, rocid, slot, module_id, chan),
          pulse_number(pulse_number), pulse_integral(pulse_integral) {}
};

#endif // FADC250HALLBPULSEINTEGRALHIT_H
