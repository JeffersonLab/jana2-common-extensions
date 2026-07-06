#include "ModuleParser_TIScaler.h"
#include <JANA/JException.h>

/**
 * @brief Parse a raw data block and extract scaler hits
 *
 * This method parses the raw FADC scaler data block by:
 * 1. Processing data words sequentially
 * 2. Interpreting each group of words as a scaler record
 * 3. Filling TIScalerHit objects and storing them in EventHits_TIScaler
 *
 * @param data_block The data block to parse
 * @param rocid ROC ID for this data block
 * @param physics_events Reference to physics events vector (will be updated)
 * @param trigger_data Trigger data for the EVIO block
 */
void ModuleParser_TIScaler::parse(std::shared_ptr<evio::BaseStructure> data_block,
                                  uint32_t rocid,
                                  std::vector<PhysicsEvent*>& physics_events,
                                  TriggerData& trigger_data) {

    // There will be only one PhysicsEvent per block for this scaler bank
    PhysicsEvent* event = new PhysicsEvent();

    auto event_hits = std::make_shared<EventHits_TIScaler>();

    // Get all data words from the block
    std::vector<uint32_t> data_words = data_block->getUIntData();

    uint32_t event_number = 0;

    auto* hit = new TIScalerHit();
    hit->rocid = rocid;
    // Sanity check: need at least 1 header word + 12 scaler words
    if (data_words.size() < 13) {
        throw JException("ModuleParser_TIScaler::parse: Not enough words (%zu) for TI scaler bank", data_words.size());
    }

    // Parse header word
    auto header_word = data_words[0];
    hit->words_idx     = getBitsInRange(header_word, 31, 16);
    hit->slot          = getBitsInRange(header_word, 15, 8);
    hit->nscalerwords  = getBitsInRange(header_word, 7, 0);

    // Parse scaler words (layout fixed by hardware definition)
    hit->live_time                = data_words[1];
    hit->busy_time                = data_words[2];
    hit->ts_input_1               = data_words[3];
    hit->ts_input_2               = data_words[4];
    hit->ts_input_3               = data_words[5];
    hit->ts_input_4               = data_words[6];
    hit->ts_input_5               = data_words[7];
    hit->ts_input_6               = data_words[8];
    hit->all_triggers_before_busy = data_words[9];
    hit->ts_inputs_before_busy    = data_words[12];
    hit->ti_event_number          = (static_cast<uint64_t>(data_words[10]) << 16) | static_cast<uint64_t>(data_words[11]);

    event_hits->scalers.push_back(hit);

    // Event number is encoded as a 48-bit value: high 16 bits then low 32 bits
/*    uint64_t event_number = (static_cast<uint64_t>(data_words[10]) << 16)
                          | static_cast<uint64_t>(data_words[11]);
*/

    event_number = trigger_data.last_event_number;

    event->SetEventNumber(event_number);

//    event->SetEventNumber(static_cast<int>(event_number));


    LOG_DEBUG(GetLogger()) << "ModuleParser_TIScaler::DEBUG - ROCID = " << rocid<< "; Get Event Number = " << event->GetEventNumber() << "; TI Event Number = " << trigger_data.last_event_number << LOG_END;

    event->addHits(event_hits);
    physics_events.push_back(event);
}

