#include "ModuleParser_FADCScaler.h"
#include <JANA/JException.h>

/**
 * @brief Parse a raw data block and extract scaler hits
 *
 * This method parses the raw FADC scaler data block by:
 * 1. Processing data words sequentially
 * 2. Interpreting each group of words as a scaler record
 * 3. Filling FADCScalerHit objects and storing them in EventHits_FADCScaler
 *
 * @param data_block The data block to parse
 * @param rocid ROC ID for this data block
 * @param physics_events Reference to physics events vector (will be updated)
 * @param trigger_data Trigger data for the EVIO block
 */
void ModuleParser_FADCScaler::parse(std::shared_ptr<evio::BaseStructure> data_block,
                                  uint32_t rocid,
                                  std::vector<PhysicsEvent*>& physics_events,
                                  TriggerData& trigger_data) {

    // There will be only one PhysicsEvent per block for this scaler bank
    PhysicsEvent* event = new PhysicsEvent();

    auto event_hits = std::make_shared<EventHits_FADCScaler>();

    // Get all data words from the block
    std::vector<uint32_t> data_words = data_block->getUIntData();

    uint32_t event_index = 0;
    uint32_t event_number = 0;

    // One bank can have multiple hits, each with different slot number
    for (size_t i = 0; i < data_words.size(); i++) {
        auto* hit = new FADCScalerHit();
        hit->rocid = rocid;

        // Parse header word
        hit->words_idx = getBitsInRange(data_words[i], 31, 16);
        hit->slot      = getBitsInRange(data_words[i], 15, 8);
        hit->ncounts   = getBitsInRange(data_words[i], 7, 0);

        LOG_DEBUG(GetLogger()) << "ModuleParser_FADCScaler::DEBUG - ROCID = " << hit->rocid << "; Words = " << hit->words_idx  <<  "; Slot = " << hit->slot << "; # of Counts = " << hit->ncounts << LOG_END;

        // Parse following scaler count words
        for (int j = 0; j < 16 && (i + 1) < data_words.size(); j++) {
            hit->counts[j] = data_words[++i];
        }
        event_hits->scalers.push_back(hit);
    }

/*    uint64_t tmp_evtnumber = 0;

    tmp_evtnumber =
    static_cast<uint64_t>(data_words[data_words.size() - 1]) |
    (static_cast<uint64_t>(data_words[data_words.size() - 2]) << 32);

    event->SetEventNumber(tmp_evtnumber);
*/

    // Compute event number and get or create the hits container
    event_number = trigger_data.last_event_number;

    event->SetEventNumber(event_number);

    // --- mirrors the processor's "Event " print, but at the source ---
    LOG_DEBUG(GetLogger()) << "ModuleParser_FADCScaler::DEBUG - Event Number = " << event->GetEventNumber() << "; Last Event Number = " << trigger_data.last_event_number << "; ROCID = " << rocid <<  LOG_END;

    event->addHits(event_hits);
    physics_events.push_back(event);
}
