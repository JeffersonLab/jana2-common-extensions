#include "ModuleParser_MPD.h"
#include "EventHits_MPD.h"
#include <JANA/JException.h>

/**
 * @brief Parse a raw data block and extract hits
 * 
 * This method parses the raw MPD data block by:
 * 1. Processing data words sequentially
 * 2. Identifying different word types (headers, trailers, data)
 * 3. Extracting MPD hits
 * 4. Adding hits to the event hits container
 * 
 * @param data_block The data block to parse
 * @param rocid ROC ID for this data block
 * @param physics_events Reference to physics events vector (will be updated)
 */
void ModuleParser_MPD::parse(std::shared_ptr<evio::BaseStructure> data_block,
                               uint32_t rocid,
                               std::vector<PhysicsEvent*>& physics_events,
                               TriggerData& trigger_data) {
    // Get all data words from the block
    std::vector<uint32_t> data_words = data_block->getUIntData();
    
    // Variables to track block and event information
    uint32_t block_slot = 0;
    int block_nevents = -1;  // -1 indicates no block header processed yet
    uint32_t event_number = 0;
    uint32_t trigger_num=0;
    uint64_t event_timestamp = 0;
    int event_index = 0;
    

    MPDHit* current_mpd_hit = nullptr; // one event has one mpd hit only

    // Map from event_number to EventHits_MPD, used to merge hits from
    // multiple blocks within the same data bank into a single PhysicsEvent.
    std::map<uint64_t, std::shared_ptr<EventHits_MPD>> event_hits_map;

    // Process each data word sequentially
    for (size_t j = 0; j < data_words.size(); j++) {
        auto d = data_words[j];
        uint32_t word_type = getBitsInRange(d, 31, 31);
        
        // Process data type defining words (word_type == 1)
        if (word_type == 1) {
            uint32_t data_type = getBitsInRange(d, 30, 27);
            
            if (data_type == 0) { // Block header
                block_slot = getBitsInRange(d, 26, 22);
                block_nevents = getBitsInRange(d, 7, 0);
            } else if (data_type == 1) { // Block trailer              
                if (block_nevents != 0) {
                    throw JException(
                        "ModuleParser_MPD::parse: Invalid data format — block trailer word before reading in all events"
                    );
                }

                auto block_trailer_slot = getBitsInRange(d, 26, 22);
                if (block_trailer_slot != block_slot) {
                    throw JException(
                        "ModuleParser_MPD::parse: Invalid data — block trailer slot(%d) != block slot(%d)",
                        block_trailer_slot, block_slot
                    );
                }

                block_nevents = -1;
                event_index = 0;
            } else if (data_type == 2) { // Event header
                if (block_nevents <= 0) {
                    throw JException(
                        "ModuleParser_MPD::parse: Invalid data format — event header before block header"
                    );
                }
                block_nevents--;

                trigger_num = getBitsInRange(d, 15, 0);
                // Compute event number and get or create the hits container
                event_number = trigger_data.first_event_number + event_index;
                event_index++;
                if( event_number != trigger_num){
                    // Other levels allowed are: LOG_ERROR, LOG_INFO, LOG_DEBUG, LOG_TRACE
                    // Refer to https://jeffersonlab.github.io/JANA2/#/howto/configuration for more information
                    LOG_WARN(GetLogger()) << "ModuleParser_MPD::parse: Warning - event number " << event_number << " != trigger_num " << trigger_num << LOG_END;
                }

                if (event_hits_map.find(event_number) == event_hits_map.end()) {
                    event_hits_map[event_number] = std::make_shared<EventHits_MPD>();
                }

            } else if (data_type == 3) { // Trigger time
                if (block_nevents < 0) {
                    throw JException(
                        "ModuleParser_MPD::parse: Invalid data format — trigger time word before block & event header"
                    );
                }
                auto trigger_time_low = getBitsInRange(d, 23, 0);
                auto d2 = data_words[++j];
                auto trigger_time_high = getBitsInRange(d2, 23, 0);
                event_timestamp = (trigger_time_high << 24) | trigger_time_low;
            } else if (data_type == 5) { // MPD Frame
                if (block_nevents < 0) {
                    throw JException(
                        "ModuleParser_MPD::parse: Invalid data format — mpd frame word before block & event header"
                    );
                }
                auto fiber_id = getBitsInRange(d, 20, 16);
                auto mpd_id = getBitsInRange(d, 4, 0);

                auto d2 = data_words[j+1];
		while ( getBitsInRange(d2, 31, 31)==0 ){  // 3*N APV sample words

	              MPDHit* current_mpd_hit = new MPDHit();
                      current_mpd_hit->rocid = rocid;
                      current_mpd_hit->slot = block_slot;
                      current_mpd_hit->trigger_num = event_number;
		      current_mpd_hit->trigger_time = event_timestamp;

		      current_mpd_hit->fiber_id = fiber_id;
                      current_mpd_hit->mpd_id = mpd_id;

                      auto apv_channel_low = getBitsInRange(d2, 30, 26);
                      current_mpd_hit->apv_samples[0] = getBitsInRange(d2, 12, 0);
                      current_mpd_hit->apv_samples[1] = getBitsInRange(d2, 25, 13);

                      auto d3 = data_words[j+2];
                      auto apv_channel_high = getBitsInRange(d3, 30, 26);
                      current_mpd_hit->apv_samples[2] = getBitsInRange(d3, 12, 0);
                      current_mpd_hit->apv_samples[3] = getBitsInRange(d3, 25, 13);

                      current_mpd_hit->apv_channel = (apv_channel_high << 5) | apv_channel_low;

                      auto d4 = data_words[j+3];
                      current_mpd_hit->apv_id = getBitsInRange(d4, 30, 26);
                      current_mpd_hit->apv_samples[4] = getBitsInRange(d4, 12, 0);
                      current_mpd_hit->apv_samples[5] = getBitsInRange(d4, 25, 13);
		      
		      event_hits_map[event_number]->mpd.push_back(current_mpd_hit);

                      j = j+3;
		      d2 = data_words[j+1];
		}
            } 
        }
    }

    for (auto& event_hit : event_hits_map) {
        PhysicsEvent* physics_event = new PhysicsEvent(event_hit.first, event_hit.second);
        physics_events.push_back(physics_event);
    }

}
