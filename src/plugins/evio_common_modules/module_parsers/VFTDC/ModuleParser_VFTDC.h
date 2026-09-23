#ifndef MODULEPARSER_VFTDC_H
#define MODULEPARSER_VFTDC_H

#include "BankParser.h"
#include "VFTDCHit.h"
#include "EventHits_VFTDC.h"
/**
 * @class ModuleParser_VFTDC
 * @brief BankParser implementation for VFTDC data
 */
class ModuleParser_VFTDC : public BankParser {
public:
    void parse(std::shared_ptr<evio::BaseStructure> data_block,
               uint32_t rocid,
               std::vector<PhysicsEvent*>& physics_events,
               TriggerData& trigger_data) override;
    
};

#endif // MODULEPARSER_VFTDC_H
