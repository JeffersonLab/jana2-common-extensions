#include "ModuleParser_faV3compton.h"
#include "EventHits_faV3compton.h"
#include <JANA/JException.h>

/**
 * @brief Parse a raw data block and extract hits
 * 
 * This method parses the raw faV3compton250 data block by:
 * 1. Processing data words sequentially
 * 2. Identifying different word types (headers, trailers, data)
 * 3. Extracting waveform and pulse hits
 * 4. Adding hits to the event hits container
 * 
 * @param data_block The data block to parse
 * @param rocid ROC ID for this data block
 * @param physics_events Reference to physics events vector (will be updated)
 * @param trigger_data Trigger metadata for this EVIO block
 */
void ModuleParser_faV3compton::parse(std::shared_ptr<evio::BaseStructure> data_block,
                               uint32_t rocid,
                               std::vector<PhysicsEvent*>& physics_events,
                               TriggerData& trigger_data) {
    // Get all data words from the block
    std::vector<uint32_t> data_words = data_block->getUIntData();

    uint32_t last_data_type = 999;
    uint32_t nwords=0;

    uint32_t block_slot = 0;
    uint32_t module_id = 0;
    uint32_t block_num = 0;
    uint32_t block_nevt = 0;
    uint32_t trigger_num = 0;
    uint32_t timestamp1 = 0;
    uint32_t timestamp2 = 0;
    
    // Map from event_number to EventHits_faV3compton, used to merge hits from
    // multiple blocks within the same data bank into a single PhysicsEvent.
    std::map<uint64_t, std::shared_ptr<EventHits_faV3compton>> event_hits_map;

    // Process each data word sequentially
    for (size_t j = 0; j < data_words.size(); j++) {
        auto d = data_words[j];
        uint32_t word_type = getBitsInRange(d, 31, 31);
	nwords++;

	// Process data type defining words (word_type == 1)
        if (word_type == 1) {
            uint32_t data_type = getBitsInRange(d, 30, 27);
	    // LOG_DEBUG(GetLogger()) << std::dec << "ModuleParser_faV3compton::DEBUG - Word " << nwords << " 0x" << std::hex << d << std::dec<<" - data type : " << data_type << LOG_END;

	    if(data_type == 0) { // block header
	       block_slot = getBitsInRange(d, 26, 22);
	       module_id = getBitsInRange(d, 21, 18);
	       block_num = getBitsInRange(d, 17, 8);
	       block_nevt = getBitsInRange(d, 7, 0);
               LOG_DEBUG(GetLogger()) << std::dec << "ModuleParser_faV3compton::DEBUG - Word " << nwords << " 0x" << std::hex << d << std::dec<<" - data type 0: Block slot = " << block_slot  << "; Module ID = " << module_id << "; Block number = " << block_num << "; Number of block events = " << block_nevt << LOG_END;
	    }

	    if(data_type == 1) { // block trailer
	       uint32_t tr_block_slot = getBitsInRange(d, 26, 22);
	       uint32_t tr_block_words = getBitsInRange(d, 21, 0);

               if( tr_block_slot != block_slot ) 
                   throw JException(
                        "ModuleParser_faV3compton::bloc_trailer: block trailer slot != block header slot"
                    );

               LOG_DEBUG(GetLogger()) << std::dec << "ModuleParser_faV3compton::DEBUG - Word " << nwords << " 0x" << std::hex << d << std::dec<<" - data type 1: Block Trailer = " << tr_block_slot  << "; Number of words = " << tr_block_words << LOG_END;
	    }

	    if(data_type == 2) { // event header
               uint32_t evt_slot = getBitsInRange(d, 26, 22);
	       uint32_t evt_trig_time = getBitsInRange(d, 21, 12);
	       uint32_t evt_trig_num = getBitsInRange(d, 11, 0);
	       trigger_num = evt_trig_num;

               if( evt_slot != block_slot ) 
                   throw JException(
                        "ModuleParser_faV3compton::Event Header: event header slot != block header slot"
                    );


               LOG_DEBUG(GetLogger()) << std::dec << "ModuleParser_faV3compton::DEBUG - Word " << nwords << " 0x" << std::hex << d << std::dec<<" - data type 2: Event Header slot = " << evt_slot  << "; Trigger time = " << evt_trig_time << "; Trigger number = " << evt_trig_num << LOG_END;
	    }

	    if(data_type == 3) { // trigger time word 1
	       timestamp1 =  getBitsInRange(d, 23, 0);

               LOG_DEBUG(GetLogger()) << std::dec << "ModuleParser_faV3compton::DEBUG - Word " << nwords << " 0x" << std::hex << d << std::dec<<" - data type 3: Trigger Time 1 = " << timestamp1 << LOG_END;
	    }

	    if(data_type == 8) { // Helicity 
	       uint32_t helicity =  getBitsInRange(d, 26, 26);
	       uint32_t tstopbeyongmps =  getBitsInRange(d, 25, 25);
	       uint32_t hel_num1 =  getBitsInRange(d, 24, 0);

               LOG_DEBUG(GetLogger()) << std::dec << "ModuleParser_faV3compton::DEBUG - Word " << nwords << " 0x" << std::hex << d << std::dec<<" - data type 8: Helicity Polarity = " << helicity << "; TstopExtendBeyondMps = " << tstopbeyongmps << "; Helicity Number 1 = " << hel_num1 << LOG_END;
	    }

	    if(data_type == 10) { // Accumulators 

               LOG_DEBUG(GetLogger()) << std::dec << "ModuleParser_faV3compton::DEBUG - Word " << nwords << " 0x" << std::hex << d << std::dec<<" - data type 10 Word 1" << LOG_END;
	    }

	    last_data_type = data_type;
        }
	else{
              if( last_data_type == 0){
                  uint32_t NSB = getBitsInRange(d, 17, 9);
                  uint32_t NSA = getBitsInRange(d, 8, 0);

                  LOG_DEBUG(GetLogger()) << std::dec << "ModuleParser_faV3compton::DEBUG - Word " << nwords << " 0x" << std::hex << d << std::dec<<" - data type 0: NSB = " << NSB  << "; NSA = " << NSA << LOG_END;
	      }

	    if( last_data_type == 3) { // trigger time word 2
	       timestamp2 =  getBitsInRange(d, 23, 0);

               LOG_DEBUG(GetLogger()) << std::dec << "ModuleParser_faV3compton::DEBUG - Word " << nwords << " 0x" << std::hex << d << std::dec<<" - data type 3: Trigger Time 2 = " << timestamp2 << LOG_END;
	    }

	    if( last_data_type == 8) { // Helicity 
	       uint32_t hel_num2 =  getBitsInRange(d, 4, 0);

               LOG_DEBUG(GetLogger()) << std::dec << "ModuleParser_faV3compton::DEBUG - Word " << nwords << " 0x" << std::hex << d << std::dec<<" - data type 8: Helicity Number 2 = " << hel_num2 << LOG_END;
	    }

	    if( last_data_type == 10) { // Accumulators  word 2
	       uint32_t acc_samp_overflow =  getBitsInRange(d, 8, 8);
	       uint32_t acc_samp_underflow =  getBitsInRange(d, 7, 7);
	       uint32_t acc_type = getBitsInRange(d, 6, 4);
	       uint32_t acc_chan = getBitsInRange(d, 3, 0);

               LOG_DEBUG(GetLogger()) << std::dec << "ModuleParser_faV3compton::DEBUG - Word " << nwords << " 0x" << std::hex << d << std::dec<<" - data type 10 Word 2: Sample Overflow = " << acc_samp_overflow << "; Sample Underflow = " << acc_samp_underflow << "; Accumulator Type = " << acc_type << "; ADC Channel = " << acc_chan << LOG_END;
	    }

	    if( last_data_type == 10) { // Accumulators  word 3
	       uint32_t acc_overflow_timestamp1 = getBitsInRange(d, 23, 0);

               LOG_DEBUG(GetLogger()) << std::dec << "ModuleParser_faV3compton::DEBUG - Word " << nwords << " 0x" << std::hex << d << std::dec<<" - data type 10 Word 3: timestamp1 of first overflow sample = " << acc_overflow_timestamp1 << LOG_END;
	    }

	    if( last_data_type == 10) { // Accumulators  word 4
	       uint32_t acc_overflow_timestamp2 = getBitsInRange(d, 23, 0);

               LOG_DEBUG(GetLogger()) << std::dec << "ModuleParser_faV3compton::DEBUG - Word " << nwords << " 0x" << std::hex << d << std::dec<<" - data type 10 Word 4: timestamp2 of first overflow sample = " << acc_overflow_timestamp2 << LOG_END;
	    }

	    if( last_data_type == 10) { // Accumulators  word 5
	       uint32_t acc_underflow_timestamp1 = getBitsInRange(d, 23, 0);

               LOG_DEBUG(GetLogger()) << std::dec << "ModuleParser_faV3compton::DEBUG - Word " << nwords << " 0x" << std::hex << d << std::dec<<" - data type 10 Word 5: timestamp1 of first underflow sample = " << acc_underflow_timestamp1 << LOG_END;
	    }

	    if( last_data_type == 10) { // Accumulators  word 6
	       uint32_t acc_underflow_timestamp2 = getBitsInRange(d, 23, 0);

               LOG_DEBUG(GetLogger()) << std::dec << "ModuleParser_faV3compton::DEBUG - Word " << nwords << " 0x" << std::hex << d << std::dec<<" - data type 10 Word 6: timestamp2 of first underflow sample = " << acc_underflow_timestamp2 << LOG_END;
	    }

	    if( last_data_type == 10) { // Accumulators  word 7
	       uint32_t acc_sum_nsample1 = getBitsInRange(d, 25, 0);

               LOG_DEBUG(GetLogger()) << std::dec << "ModuleParser_faV3compton::DEBUG - Word " << nwords << " 0x" << std::hex << d << std::dec<<" - data type 10 Word 7: LSBytes number of samples in sum = " << acc_sum_nsample1 << LOG_END;
	    }

	    if( last_data_type == 10) { // Accumulators  word 8
	       uint32_t acc_sum_nsample2 = getBitsInRange(d, 6, 0);

               LOG_DEBUG(GetLogger()) << std::dec << "ModuleParser_faV3compton::DEBUG - Word " << nwords << " 0x" << std::hex << d << std::dec<<" - data type 10 Word 7: MSBytes number of samples in sum = " << acc_sum_nsample2 << LOG_END;
	    }

	    if( last_data_type == 10) { // Accumulators  word 9
	       uint32_t acc_sum1 = getBitsInRange(d, 29, 0);

               LOG_DEBUG(GetLogger()) << std::dec << "ModuleParser_faV3compton::DEBUG - Word " << nwords << " 0x" << std::hex << d << std::dec<<" - data type 10 Word 7: LSBytes sum = " << acc_sum1 << LOG_END;
	    }

	    if( last_data_type == 10) { // Accumulators  word 10
	       uint32_t acc_sum2 = getBitsInRange(d, 4, 0);

               LOG_DEBUG(GetLogger()) << std::dec << "ModuleParser_faV3compton::DEBUG - Word " << nwords << " 0x" << std::hex << d << std::dec<<" - data type 10 Word 10: MSBytes sum = " << acc_sum2 << LOG_END;
	    }

	    if( last_data_type == 10) { // Accumulators  word 11
	       uint32_t acc_np_nsboverlapped = getBitsInRange(d, 27, 14);
	       uint32_t acc_np_nonsa = getBitsInRange(d, 13, 0);

               LOG_DEBUG(GetLogger()) << std::dec << "ModuleParser_faV3compton::DEBUG - Word " << nwords << " 0x" << std::hex << d << std::dec<<" - data type 10 Word 11: number of pulses have NSB overlapped = " << acc_np_nsboverlapped << "; number of pulses have no NSA cross" << acc_np_nonsa << LOG_END;
	    }

	    if( last_data_type == 10) { // Accumulators  word 12
	       uint32_t acc_np_miss = getBitsInRange(d, 13, 0);

               LOG_DEBUG(GetLogger()) << std::dec << "ModuleParser_faV3compton::DEBUG - Word " << nwords << " 0x" << std::hex << d << std::dec<<" - data type 10 Word 12: total missed pulses = " << acc_np_miss << LOG_END;
	    }


	}
        
    }

    // Create PhysicsEvent objects from the map
    for (auto& event_hit : event_hits_map) {
        PhysicsEvent* physics_event = new PhysicsEvent(event_hit.first, event_hit.second);
        physics_events.push_back(physics_event);
    }


}


