#ifndef VFTDCHIT_H
#define VFTDCHIT_H

#include "DAQAddress.h"

/**
 * @class VFTDCHit
 * @brief Class for VFTDC hits
 */
class VFTDCHit {
public:
    uint32_t rocid;
    uint32_t slot;
    uint32_t board_id;
    uint64_t timestamp;
    uint32_t group_num;
    uint32_t channel_num;
    uint32_t edge_type;
    uint32_t coarse_time;
    uint32_t fine_time;
    uint32_t two_ns;

    /**
     * @brief Default constructor
     * 
     * Initializes all members to zero.
     */
    VFTDCHit() : rocid(0), slot(0), board_id(0), timestamp(0), group_num(0), channel_num(0), edge_type(0), coarse_time(0), fine_time(0), two_ns(0) {}
    
    /**
     * @brief Parameterized constructor
     * 
     * Initializes all members with the provided values.
     * 
     * @param rocid Readout Controller ID
     * @param slot Slot number in the crate
     * @param board_id Board ID
     * @param timestamp Timestamp
     * @param group_num Group number
     * @param channel_num Channel number
     * @param edge_type Edge type
     * @param coarse_time Coarse time
     * @param fine_time Fine time
     * @param two_ns Two ns
     */
    VFTDCHit(uint32_t rocid, uint32_t slot, uint32_t board_id, uint64_t timestamp, uint32_t group_num, uint32_t channel_num, uint32_t edge_type, uint32_t coarse_time, uint32_t fine_time, uint32_t two_ns)
        : rocid(rocid), slot(slot), board_id(board_id), timestamp(timestamp), group_num(group_num), channel_num(channel_num), edge_type(edge_type), coarse_time(coarse_time), fine_time(fine_time), two_ns(two_ns) {}
};

inline DAQAddress getDAQAddress(const VFTDCHit& hit) {
    return {hit.rocid, hit.slot, hit.channel_num};
}

#endif // VFTDCHIT_H
