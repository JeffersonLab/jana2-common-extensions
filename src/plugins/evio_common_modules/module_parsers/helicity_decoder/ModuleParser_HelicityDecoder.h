#ifndef MODULEPARSER_HELICITYDECODER_H
#define MODULEPARSER_HELICITYDECODER_H

#include "ModuleParser.h"
#include "HelicityDecoderData.h"
#include "EventHits_HelicityDecoder.h"

/**
 * @class ModuleParser_HelicityDecoder
 * @brief ModuleParser implementation for helicity decoder data
 *
 * This parser decodes helicity decoder board words from an EVIO bank and fills
 * an EventHits_HelicityDecoder container with HelicityDecoderData objects.
 */
class ModuleParser_HelicityDecoder : public ModuleParser {
public:
    void parse(std::shared_ptr<evio::BaseStructure> data_block,
               uint32_t rocid,
               std::vector<PhysicsEvent*>& physics_events,
               TriggerData& trigger_data) override;
 
private:

    /**
     * Parse decoder data from data words
     * @param data_words Vector of data words
     * @param index Current index in data_words (will be updated)
     * @param trigger_num Trigger number
     * @param timestamp1 First timestamp
     * @param timestamp2 Second timestamp
     * @param rocid ROC ID
     * @param slot Slot number
     * @param module_id Module ID
     * @return Parsed helicity decoder data
     */
    HelicityDecoderData parseDecoderData(
        const std::vector<uint32_t>& data_words,
        size_t& index,
        uint32_t trigger_num,
        uint32_t timestamp1,
        uint32_t timestamp2,
        uint32_t rocid,
        uint32_t slot,
        uint32_t module_id,
	uint32_t nwords
    );

};

#endif // MODULEPARSER_HELICITYDECODER_H

