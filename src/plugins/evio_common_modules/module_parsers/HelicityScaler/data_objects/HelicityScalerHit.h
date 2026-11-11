#ifndef HELICITYSCALERHIT_H
#define HELICITYSCALERHIT_H

/**
 * @class HelicityScalerHit
 * @brief Hit class for Helicity scaler data
 * 
 * This class contains the data members for one Helicity scaler record.
 */
class HelicityScalerHit {
public:
    uint32_t rocid;
    uint32_t userbit1;
    uint32_t userbit2;
    uint32_t chan;
    uint32_t count;

    /// Default constructor: initializes all members to zero.
    HelicityScalerHit()
        : rocid(0), userbit1(0), userbit2(0), chan(0), count(0) {}

    /// Constructor initializing all members explicitly.
    HelicityScalerHit(uint32_t rocid, uint32_t userbit1, uint32_t userbit2, uint32_t chan, uint32_t count)
        : rocid(rocid), userbit1(userbit1), userbit2(userbit2), chan(chan),  count(count) {}
    
    /**
     * @brief Virtual destructor
     * 
     * Ensures proper cleanup of derived classes.
     */
    virtual ~HelicityScalerHit() = default;
};

#endif // HELICITYSCALERHIT_H
