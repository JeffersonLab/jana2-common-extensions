#ifndef faV3comptonHIT_H
#define faV3comptonHIT_H

/**
 * @class faV3comptonHit
 * @brief Base class for all faV3compton hits
 * 
 * This class contains the common data members that are shared by all
 * types of faV3compton hits (accumulators, pulse hits, waveforms etc.). 
 */
class faV3comptonHit {
public:
    uint64_t trigger_num;
    uint32_t timestamp1;
    uint32_t timestamp2;
    uint32_t rocid;
    uint32_t slot;
    uint32_t module_id;
    uint32_t chan;

    /**
     * @brief Default constructor
     * 
     * Initializes all members to zero.
     */
    faV3comptonHit() : trigger_num(0), timestamp1(0), timestamp2(0), 
            rocid(0), slot(0), module_id(0), chan(0) {}
    
    /**
     * @brief Parameterized constructor
     * 
     * Initializes all members with the provided values.
     * 
     * @param trigger_num Trigger number for this hit
     * @param timestamp1 First timestamp value
     * @param timestamp2 Second timestamp value
     * @param rocid Readout Controller ID
     * @param slot Slot number in the crate
     * @param module_id Module identifier
     * @param chan Channel number
     */
    faV3comptonHit(uint64_t trigger_num, uint64_t timestamp1, uint64_t timestamp2,
        uint32_t rocid, uint32_t slot, uint32_t module_id, uint32_t chan)
        : trigger_num(trigger_num), timestamp1(timestamp1), timestamp2(timestamp2),
          rocid(rocid), slot(slot), module_id(module_id), chan(chan) {}
    
    /**
     * @brief Virtual destructor
     * 
     * Ensures proper cleanup of derived classes.
     */
    virtual ~faV3comptonHit() = default;
};

#endif // faV3comptonHIT_H
