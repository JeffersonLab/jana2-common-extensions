#ifndef MODULEPARSER_FADC_H
#define MODULEPARSER_FADC_H

#include "ModuleParser.h"
#include "FADC250WaveformHit.h"
#include "FADC250PulseHit.h"

/**
 * @class ModuleParser_FADC
 * @brief ModuleParser implementation for FADC250 data
 */
class ModuleParser_FADC : public ModuleParser {
public:
    void parse(std::shared_ptr<evio::BaseStructure> data_block,
               uint32_t rocid,
               std::vector<PhysicsEvent*>& physics_events,
               TriggerData& trigger_data) override;

private:

    /**
     * Parse waveform data from data words
     * @param data_words Vector of data words
     * @param index Current index in data_words (will be updated)
     * @param trigger_num Trigger number
     * @param timestamp1 First timestamp
     * @param timestamp2 Second timestamp
     * @param rocid ROC ID
     * @param slot Slot number
     * @param module_id Module ID
     * @param chan Channel number
     * @param waveform_len Expected waveform length
     * @return Parsed waveform hit
     */
    FADC250WaveformHit parseWaveformData(
        const std::vector<uint32_t>& data_words,
        size_t& index,
        uint32_t trigger_num,
        uint32_t timestamp1,
        uint32_t timestamp2,
        uint32_t rocid,
        uint32_t slot,
        uint32_t module_id,
        uint32_t chan,
        uint32_t waveform_len
    );
    
    /**
     * Parse pulse data from data words
     * @param data_words Vector of data words
     * @param index Current index in data_words (will be updated)
     * @param trigger_num Trigger number
     * @param timestamp1 First timestamp
     * @param timestamp2 Second timestamp
     * @param rocid ROC ID
     * @param slot Slot number
     * @param module_id Module ID
     * @param chan Channel number
     * @param pedestal_quality Pedestal quality
     * @param pedestal_sum Pedestal sum
     * @return Vector of parsed pulse hits
     */
    std::vector<FADC250PulseHit> parsePulseData(
        const std::vector<uint32_t>& data_words,
        size_t& index,
        uint32_t trigger_num,
        uint32_t timestamp1,
        uint32_t timestamp2,
        uint32_t rocid,
        uint32_t slot,
        uint32_t module_id,
        uint32_t chan,
        uint32_t pedestal_quality,
        uint32_t pedestal_sum
    );
};

#endif // MODULEPARSER_FADC_H