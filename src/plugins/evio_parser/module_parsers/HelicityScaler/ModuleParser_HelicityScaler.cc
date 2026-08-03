#include "ModuleParser_HelicityScaler.h"
#include <JANA/JException.h>

/**
 * @brief Parse a raw data block and extract scaler hits
 *
 * This method parses the raw Helicity scaler data block by:
 * 1. Processing data words sequentially
 * 2. Interpreting each group of words as a scaler record
 * 3. Filling HelicityScalerHit objects and storing them in EventHits_HelicityScaler
 *
 * @param data_block The data block to parse
 * @param rocid ROC ID for this data block
 * @param physics_events Reference to physics events vector (will be updated)
 * @param trigger_data Trigger data for the EVIO block
 */
void ModuleParser_HelicityScaler::parse(std::shared_ptr<evio::BaseStructure> data_block,
                                  uint32_t rocid,
                                  std::vector<PhysicsEvent*>& physics_events,
                                  TriggerData& trigger_data) {

    // There will be only one PhysicsEvent per block for this scaler bank
    PhysicsEvent* event = new PhysicsEvent();

    auto event_hits = std::make_shared<EventHits_HelicityScaler>();

    // Get all data words from the block
    std::vector<uint32_t> data_words = data_block->getUIntData();

    uint32_t event_index = 0;
    uint64_t event_number = 0;

    // One bank can have multiple hits, each with different slot number
    for (size_t i = 0; i < data_words.size(); i++) {
        auto* hit = new HelicityScalerHit();
        hit->rocid = rocid;

        // Parse header word
        hit->userbit1 = getBitsInRange(data_words[i], 31, 31);
        hit->userbit2 = getBitsInRange(data_words[i], 30, 30);
        hit->chan     = getBitsInRange(data_words[i], 28, 24);
        hit->count    = getBitsInRange(data_words[i], 23, 0);

        LOG_DEBUG(GetLogger()) << "ModuleParser_HelicityScaler::DEBUG - ROCID = " << hit->rocid << "; Userbit1 = " << hit->userbit1  <<  "; Userbit2 = " << hit->userbit2 << "; Channel = " << hit->chan << "; Count = " << hit->count << LOG_END;

        event_hits->scalers.push_back(hit);
    }


    // Compute event number and get or create the hits container
    event_number = trigger_data.last_event_number;

    event->SetEventNumber(event_number);

    // --- mirrors the processor's "Event " print, but at the source ---
    LOG_DEBUG(GetLogger()) << "ModuleParser_HelicityScaler::DEBUG - Event Number = " << event->GetEventNumber() << "; Last Event Number = " << trigger_data.last_event_number << "; ROCID = " << rocid <<  LOG_END;

    event->addHits(event_hits);
    physics_events.push_back(event);
} 

