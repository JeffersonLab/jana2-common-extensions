#ifndef FADC250SCALERHIT_H
#define FADC250SCALERHIT_H

/**
 * @class FADC250ScalerHit
 * @brief FADC250 scaler data
 */
class FADC250ScalerHit {
public:
    uint32_t roc;
    uint32_t slot;
    uint32_t counts[16];
    uint32_t trignum;
    uint32_t timer;

    /**
     * @brief Default constructor
     * 
     * Initializes all members to zero.
     */
    FADC250ScalerHit() : roc(0), slot(0), counts{0}, trignum(0), timer(0) {}

    
    /**
     * @brief Parameterized constructor
     * 
     * Initializes the base hit information and basic pulse data.
     * Other pulse analysis data is typically filled in during parsing.
     */

    FADC250ScalerHit(uint32_t rocid_, uint32_t slot_, uint32_t trignum_, uint32_t timer_)
        : roc(rocid_), slot(slot_), trignum(trignum), timer(timer_), counts{0} {}


};

#endif // FADC250SCALERHIT_H
