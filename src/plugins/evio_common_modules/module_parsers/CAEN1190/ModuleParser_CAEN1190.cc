#include "ModuleParser_CAEN1190.h"
#include "EventHits_CAEN1190.h"
#include <JANA/JException.h>

/**
 * @brief Parse a raw data block and extract hits
 * 
 * This method parses the raw CAEN data block by:
 * 1. Processing data words sequentially
 * 2. Identifying different word types (headers, trailers, data)
 * 3. Extracting CAEN hits
 * 4. Adding hits to the event hits container
 * 
 * @param data_block The data block to parse
 * @param rocid ROC ID for this data block
 * @param physics_events Reference to physics events vector (will be updated)
 * @param trigger_data Trigger metadata for this EVIO block
 */
void ModuleParser_CAEN1190::parse(std::shared_ptr<evio::BaseStructure> data_block,
                               uint32_t rocid,
                               std::vector<PhysicsEvent*>& physics_events,
                               TriggerData& trigger_data) {
    // Get all data words from the block
    std::vector<uint32_t> data_words = data_block->getUIntData();
    
    // Variables to track block and event information

    uint32_t chan = 0;
    uint32_t measurement = 0;
    uint32_t opt = 0;
    uint32_t flags = 0;
    uint32_t trig_time = 0;
    uint32_t hdr_chip_id = 0;
    uint32_t hdr_event_id = 0;
    uint32_t hdr_bunch_id = 0;
    uint32_t glb_hdr_event = 0;
    uint32_t glb_hdr_slot = 0;
    uint32_t glb_trl_word = 0;
    uint32_t glb_trl_slot = 0;
    uint32_t glb_trl_status = 0;
    uint32_t err_chip_id = 0;
    uint32_t trl_chip_id = 0;
    uint32_t trl_event_id = 0;
    uint32_t trl_word_cnt = 0;
    uint32_t event_index = 0;
    uint32_t event_number = 0;

    // Map from event_number to EventHits_CAEN1190, used to merge hits from
    // multiple blocks within the same data bank into a single PhysicsEvent.
    std::map<uint64_t, std::shared_ptr<EventHits_CAEN1190>> event_hits_map;

    LOG_DEBUG(GetLogger()) << "ModuleParser_CAEN1190::DEBUG - Initialize Data  ROCID = " << rocid  <<  "; First Event Number = " << trigger_data.first_event_number << LOG_END;

    // Process each data word sequentially
    for (size_t j = 0; j < data_words.size(); j++) {
        auto d = data_words[j];
        uint32_t data_type = getBitsInRange(d, 31, 27);

        if (data_type == 8) { // Global header
                glb_hdr_event = getBitsInRange(d, 26, 5);
                glb_hdr_slot = getBitsInRange(d, 4, 0);

                event_number = trigger_data.first_event_number + event_index;
                event_index++;

                if (event_hits_map.find(event_number) == event_hits_map.end()) {
                    event_hits_map[event_number] = std::make_shared<EventHits_CAEN1190>();
                }



                LOG_DEBUG(GetLogger()) << "ModuleParser_CAEN1190::DEBUG - data type 8 GLB HDR Event = " << glb_hdr_event  <<  "; GLB HDR Slot = " << glb_hdr_slot << LOG_END;
                    
            } else if (data_type == 16) { // Global trailer              
                glb_trl_status = getBitsInRange(d, 26, 24);
                glb_trl_word = getBitsInRange(d, 20, 5);
                glb_trl_slot = getBitsInRange(d, 4, 0);

                event_index = 0;

                LOG_DEBUG(GetLogger()) << "ModuleParser_CAEN1190::DEBUG - data type 16 GLB TRL Status = " << glb_trl_status  <<  "; GLB TRL Word = " << glb_trl_word << "; GLB TRL Slot = " << glb_trl_slot << LOG_END;

            } else if (data_type == 1) { // TDC header
                hdr_chip_id = getBitsInRange(d, 25, 24);
                hdr_event_id = getBitsInRange (d, 23, 12);
                hdr_bunch_id = getBitsInRange(d, 11, 0);

                LOG_DEBUG(GetLogger()) << "ModuleParser_CAEN1190::DEBUG - data type 1 HDR Chip ID = " << hdr_chip_id  <<  "; HDR Event ID = " << hdr_event_id << "; HDR Bunch ID = " << hdr_bunch_id << LOG_END;


            } else if (data_type == 0) { // TDC Measurement
                opt = getBitsInRange (d, 26, 26);
                chan = getBitsInRange (d, 25, 19);
                measurement = getBitsInRange (d, 18, 0);

                LOG_DEBUG(GetLogger()) << "ModuleParser_CAEN1190::DEBUG - data type 0 OPT = " << opt  <<  "; Channel = " << chan << "; Measurement = " << measurement << LOG_END;

                auto caen1190_hit = new CAEN1190Hit();
                caen1190_hit->rocid = rocid;
                caen1190_hit->slot  = glb_hdr_slot;
                caen1190_hit->chan = chan;
                caen1190_hit->measurement = measurement;
                caen1190_hit->opt = opt;
                caen1190_hit->flags = flags;
                caen1190_hit->trig_time = trig_time;
                caen1190_hit->hdr_chip_id = hdr_chip_id;
                caen1190_hit->hdr_event_id = hdr_event_id;
                caen1190_hit->hdr_bunch_id = hdr_bunch_id;
                caen1190_hit->glb_trl_status = glb_trl_status;
                event_hits_map[event_number]->caen_hits.push_back(caen1190_hit);

            } else if (data_type == 4) { // TDC Error
                err_chip_id = getBitsInRange(d, 25, 24);
                flags = getBitsInRange (d, 14, 0);

                LOG_DEBUG(GetLogger()) << "ModuleParser_CAEN1190::DEBUG - data type 4 ERR Chip ID = " << err_chip_id  <<  "; Flags = " << flags << LOG_END;

            } else if (data_type == 3) { // TDC Trailer
                trl_chip_id = getBitsInRange(d, 25, 24);
                trl_event_id = getBitsInRange (d, 23, 12);
                trl_word_cnt = getBitsInRange(d, 11, 0);

                LOG_DEBUG(GetLogger()) << "ModuleParser_CAEN1190::DEBUG - data type 3 TRL Chip ID = " << trl_chip_id  <<  "; TRL Event ID = " << trl_event_id << "; TRL Word Count = " << trl_word_cnt << LOG_END;

            } else if (data_type == 17) { //Trigger Time
                trig_time = getBitsInRange (d, 26, 0);

                LOG_DEBUG(GetLogger()) << "ModuleParser_CAEN1190::DEBUG - data type 17 Trigger Time = " << trig_time  << LOG_END;

            }      
        } 

    for (auto& event_hit : event_hits_map) {
        PhysicsEvent* physics_event = new PhysicsEvent(event_hit.first, event_hit.second);
        physics_events.push_back(physics_event);
    }
}
