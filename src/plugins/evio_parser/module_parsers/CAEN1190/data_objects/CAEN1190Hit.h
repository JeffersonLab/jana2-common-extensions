#ifndef CAEN1190HIT_H
#define CAEN1190HIT_H

/**
 * @class CAEN1190Hit
 * @brief Base class for all CAEN1190 hits
 */
class CAEN1190Hit {
public:
    uint32_t rocid;
    uint32_t slot;
    uint32_t chan;
    uint32_t measurement;
    uint32_t opt;
    uint32_t flags;
    uint32_t trig_time;
    uint32_t hdr_chip_id;
    uint32_t hdr_event_id;
    uint32_t hdr_bunch_id;
    uint32_t glb_trl_status;

    /**
     * @brief Default constructor
     * 
     * Initializes all members to zero.
     */
    CAEN1190Hit() : rocid(0), slot(0), chan(0), measurement(0), opt(0), flags(0),
                    trig_time(0), hdr_chip_id(0), hdr_event_id(0), hdr_bunch_id(0), glb_trl_status(0) {}
    
    /**
     * @brief Parameterized constructor
     * 
     * Initializes all members with the provided values.
     * 
     * @param rocid Readout Controller ID
     * @param slot Slot number in the crate
     * @param chan Channel number
     * @param measurement Raw measurements
     * @param opt Trail vs Lead identifier
     * @param flags Error flags
     * @param trigger_time Trigger arrival time 
     * @param hdr_chip_id Module TDC Header chip identifier 
     * @param hdr_event_id Module TDC Header event identifier 
     * @param hdr_bunch_id Module TDC Header bunch identifier 
     * @param glb_trl_status Global Trailer status 
     */
    CAEN1190Hit(uint32_t rocid, uint32_t slot, uint32_t chan, uint32_t measurement, uint32_t opt, uint32_t flags, uint32_t trig_time, uint32_t hdr_chip_id, uint32_t hdr_event_id, uint32_t hdr_bunch_id, uint32_t glb_trl_status)
        : rocid(rocid), slot(slot), chan(chan), measurement(measurement), opt(opt), flags(flags), trig_time(trig_time), hdr_chip_id(hdr_chip_id), hdr_event_id(hdr_event_id), hdr_bunch_id(hdr_bunch_id), glb_trl_status(glb_trl_status){}
    
    /**
     * @brief Virtual destructor
     * 
     * Ensures proper cleanup of derived classes.
     */
    virtual ~CAEN1190Hit() = default;
};

#endif // CAEN1190HIT_H
