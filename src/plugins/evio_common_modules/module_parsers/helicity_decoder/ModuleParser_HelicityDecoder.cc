#include "ModuleParser_HelicityDecoder.h"
#include "EventHits_HelicityDecoder.h"
#include <JANA/JException.h>

/**
 * @brief Parse a raw data block and extract hits
 * 
 * This method parses the raw helicity decoder data block by:
 * 1. Processing data words sequentially
 * 2. Identifying different word types (headers, trailers, data)
 * 3. Extracting helicity related information
 * 4. Adding the information to the event hits container
 * 
 * @param data_block The data block to parse
 * @param rocid ROC ID for this data block
 * @param physics_events Reference to physics events vector (will be updated)
 */
void ModuleParser_HelicityDecoder::parse(std::shared_ptr<evio::BaseStructure> data_block,
                               uint32_t rocid,
                               std::vector<PhysicsEvent*>& physics_events,
                               TriggerData& trigger_data) {
    // Get all data words from the block
    std::vector<uint32_t> data_words = data_block->getUIntData();
    
    // Variables to track block and event information
    uint32_t block_slot = 0;
    uint32_t module_id = 0;
    uint32_t trigger_num = 0;
    uint32_t timestamp1 = 0;
    uint32_t timestamp2 = 0;
    int block_nevents = -1;  // -1 indicates no block header processed yet
    int block_number = 0;
    int event_index = 0;
    // Use HelicityDecoder-specific hits container here, but store it later via the base
    // EventHits* inside PhysicsEvent to keep that interface generic.
    std::shared_ptr<EventHits_HelicityDecoder> event_hits = nullptr;
    
    // Process each data word sequentially
    for (size_t j = 0; j < data_words.size(); j++) {
        auto d = data_words[j];
        uint32_t word_type = getBitsInRange(d, 31, 31);
        
        // Process data type defining words (word_type == 1)
        if (word_type == 1) {
            uint32_t data_type = getBitsInRange(d, 30, 27);
            
            if (data_type == 0) { // Block header
                block_slot = getBitsInRange(d, 26, 22);
                module_id = getBitsInRange(d, 21, 18);
                block_number = getBitsInRange(d, 17, 8);
                block_nevents = getBitsInRange(d, 7, 0);

            } else if (data_type == 1) { // Block trailer              
                if (block_nevents != 0) {
                    throw JException(
                        "ModuleParser_HelicityDecoder::parse: Invalid data format — block trailer word before reading in all events"
                    );
                }
                block_nevents = -1;
                if (j == data_words.size() - 1 && event_hits != nullptr) { // because in that case event is not followed by any event header later
                    PhysicsEvent* physics_event = new PhysicsEvent(trigger_data.first_event_number + event_index, event_hits);
                    physics_events.push_back(physics_event);
                }
                event_index = 0;
            } else if (data_type == 2) { // Event header
                if (event_hits != nullptr) { // insert previous event into physics_events first
                    PhysicsEvent* physics_event = new PhysicsEvent(trigger_data.first_event_number + event_index, event_hits);
                    physics_events.push_back(physics_event);
                    event_index++;
                }
                if (block_nevents <= 0) {
                    throw JException(
                        "ModuleParser_HelicityDecoder::parse: Invalid data format — event header before block header"
                    );
                }
                block_nevents--;
                auto eh_slot = getBitsInRange(d, 26, 22);
                if (eh_slot != block_slot) {
                    throw JException(
                        "ModuleParser_HelicityDecoder::parse: Invalid data — event slot(%d) != block slot(%d)", 
                        eh_slot, block_slot
                    );
                }
                trigger_num = getBitsInRange(d, 11, 0);
                // Initialize new HelicityDecoder-specific hits container for this event
                event_hits = std::make_shared<EventHits_HelicityDecoder>();
            } else if (data_type == 3) { // Trigger time
                if (block_nevents < 0) {
                    throw JException(
                        "ModuleParser_HelicityDecoder::parse: Invalid data format — trigger time word before block & event header"
                    );
                }
                timestamp1 = getBitsInRange(d, 23, 0);
                auto d2 = data_words[++j];
                timestamp2 = getBitsInRange(d2, 23, 0);
            } else if (data_type == 8) { // decoder data header
                if (block_nevents < 0) {
                    throw JException(
                        "ModuleParser_HelicityDecoder::parse: Invalid data format — helicity decoder data word before block & event header"
                    );
                }
                uint32_t nwords = getBitsInRange(d, 5, 0);

                if (nwords != 14) {
			printf("Warning:  Event %d Helicity Decoder data words n=%d is not 14 !!\n", trigger_num, nwords);
                }
                
                // Parse pulse data and add to event hits
                auto hit = parseDecoderData(
                    data_words, j, trigger_num, timestamp1, timestamp2,
                    rocid, block_slot, module_id, nwords
                );
                event_hits->helicity.push_back(new HelicityDecoderData(hit));
            }
        }
    }
}


/**
 * @brief Parse decoder data from data words
 */
HelicityDecoderData ModuleParser_HelicityDecoder::parseDecoderData(
    const std::vector<uint32_t>& data_words,
    size_t& index,
    uint32_t trigger_num,
    uint32_t timestamp1,
    uint32_t timestamp2,
    uint32_t rocid,
    uint32_t slot,
    uint32_t module_id,
    uint32_t nwords ) {
   
    HelicityDecoderData hit;

    hit.trigger_num = trigger_num;
    hit.timestamp1 = timestamp1;
    hit.timestamp2 = timestamp2;
    hit.rocid = rocid;
    hit.slot = slot;
    hit.module_id = module_id;


    // Parse decoder data from continuation words
    for (size_t k = index + 1; k < index + 1 + nwords; k++) {
        if (k >= data_words.size()) break;
        
        auto ww = data_words[k]; // data word
	switch ( k - index ) {
             case 1:
                  hit.helicity_seed=ww;
                  break;
             case 2:
                  hit.n_tstable_fall=ww;
                  break;
             case 3:
                  hit.n_tstable_rise=ww;
                  break;
             case 4:
                  hit.n_pattsync=ww;
                  break;
             case 5:
                  hit.n_pairsync=ww;
                  break;
             case 6:
                  hit.time_tstable_start=ww;
                  break;
             case 7:
                  hit.time_tstable_end=ww;
                  break;
             case 8:
                  hit.last_tstable_duration=ww;
                  break;
             case 9:
                  hit.last_tsettle_duration=ww;
                  break;
             case 10:
                  hit.trig_tstable = getBitsInRange(ww,0,0);
                  hit.trig_pattsync = getBitsInRange(ww,1,1);
                  hit.trig_pairsync = getBitsInRange(ww,2,2);
                  hit.trig_helicity = getBitsInRange(ww,3,3);
                  hit.trig_pat0_helicity = getBitsInRange(ww,4,4);
                  hit.trig_polarity = getBitsInRange(ww,5,5);
                  hit.trig_pat_count = getBitsInRange(ww,15,8);
                  break;
             case 11:
                  hit.last32wins_pattsync=ww;
                  break;
             case 12:
                  hit.last32wins_pairsync=ww;
                  break;
             case 13:
                  hit.last32wins_helicity=ww;
                  break;
             case 14:
                  hit.last32wins_pattsync_hel=ww;
                  break;
	}
    }
        
    // Update index to skip over the decoded continuation words
    index += nwords;
    
    return hit;
}
