#ifndef HelicityDecoderDATA_H
#define HelicityDecoderDATA_H

/**
 * @class HelicityDecoderData
 * @brief Base class for all HelicityDecoder data
 * 
 * This class contains the data members for helicity decoder board
 */
class HelicityDecoderData {
public:
    uint64_t trigger_num = 0;
    uint32_t timestamp1 = 0;
    uint32_t timestamp2 = 0;
    uint32_t rocid = 0;
    uint32_t slot = 0;
    uint32_t module_id = 0;
    uint32_t helicity_seed = 0;
    uint32_t n_tstable_fall = 0;
    uint32_t n_tstable_rise = 0;
    uint32_t n_pattsync = 0;
    uint32_t n_pairsync = 0;
    uint32_t time_tstable_start = 0;
    uint32_t time_tstable_end = 0;
    uint32_t last_tstable_duration = 0;
    uint32_t last_tsettle_duration = 0;
    uint32_t trig_tstable = 0;
    uint32_t trig_pattsync = 0;
    uint32_t trig_pairsync = 0;
    uint32_t trig_helicity = 0;
    uint32_t trig_pat0_helicity = 0;
    uint32_t trig_polarity = 0;
    uint32_t trig_pat_count = 0;
    uint32_t last32wins_pattsync = 0;
    uint32_t last32wins_pairsync = 0;
    uint32_t last32wins_helicity = 0;
    uint32_t last32wins_pattsync_hel = 0;

    virtual ~HelicityDecoderData() = default;
};

#endif // HelicityDecoderDATA_H
