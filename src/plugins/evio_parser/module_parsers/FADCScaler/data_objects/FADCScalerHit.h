#ifndef FADCSCALERHIT_H
#define FADCSCALERHIT_H

#include "DAQAddress.h"

/**
 * @class FADCScalerHit
 * @brief Hit class for FADC scaler data
 * 
 * This class contains the data members for one FADC scaler record.
 */
class FADCScalerHit {
public:
    uint32_t rocid;
    uint32_t words_idx;
    uint32_t slot;
    uint32_t ncounts;
    uint32_t counts[16];

    /// Default constructor: initializes all members to zero.
    FADCScalerHit()
        : rocid(0), words_idx(0), slot(0), ncounts(0), counts{0} {}
    
    /// Constructor with basic identifiers; counts are initialized to zero.
    FADCScalerHit(uint32_t rocid_, uint32_t words_idx_, uint32_t slot_)
        : rocid(rocid_), words_idx(words_idx_), slot(slot_), ncounts(0), counts{0} {}
    
    /**
     * @brief Virtual destructor
     * 
     * Ensures proper cleanup of derived classes.
     */
    virtual ~FADCScalerHit() = default;
};

inline DAQAddress getDAQAddress(const FADCScalerHit& hit) {
    return {hit.rocid, hit.slot, DAQAddress::UnspecifiedChannel};
}

#endif // FADCSCALERHIT_H
