#ifndef MPDHIT_H
#define MPDHIT_H

#include "DAQAddress.h"

/**
 * @class MPDHit
 * @brief Class for MPD hits
 */
class MPDHit {
public:
    uint32_t rocid;
    uint32_t slot;
    uint32_t trigger_num;
    uint64_t trigger_time;
    uint32_t mpd_id;
    uint32_t fiber_id;
    uint32_t apv_channel;
    uint32_t apv_id;
    uint32_t apv_samples[6];

    /**
     * @brief Default constructor
     * 
     * Initializes all members to zero.
     */
    MPDHit() : rocid(0), slot(0), trigger_num(0), trigger_time(0), mpd_id(0), fiber_id(0), apv_channel(0), apv_id(0) {}
    
    /**
     * @brief Parameterized constructor
     * 
     * Initializes all members with the provided values.
     * 
     * @param rocid Readout Controller ID
     * @param slot Slot number in the crate
     * @param trigger_num Trigger number
     * @param trigger_time Trigger time
     * @param mpd_id MPD ID
     * @param fiber_id Fiber ID
     * @param apv_channel APV channel
     * @param apv_id APV ID
     * @param apv_samples APV samples (6 samples)
     */
    MPDHit(uint32_t rocid, uint32_t slot, uint32_t trigger_num, uint64_t trigger_time, uint32_t mpd_id, uint32_t fiber_id, uint32_t apv_channel, uint32_t apv_id, const uint32_t apv_samples_in[6])
        : rocid(rocid), slot(slot), trigger_num(trigger_num), trigger_time(trigger_time), mpd_id(mpd_id), fiber_id(fiber_id), apv_channel(apv_channel), apv_id(apv_id) {
        for (int i = 0; i < 6; i++) apv_samples[i] = apv_samples_in[i];
    }
};

inline DAQAddress getDAQAddress(const MPDHit& hit) {
    return {hit.rocid, hit.slot, hit.apv_channel};
}

#endif // MPDHIT_H
