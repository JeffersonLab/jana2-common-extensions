#ifndef MODULEPARSER_CAEN1190_H
#define MODULEPARSER_CAEN1190_H

#include "BankParser.h"
#include "CAEN1190Hit.h"
#include "EventHits_CAEN1190.h"
/**
 * @class ModuleParser_CAEN1190
 * @brief BankParser implementation for CAEN1190 data
 */
class ModuleParser_CAEN1190 : public BankParser {
public:
    void parse(std::shared_ptr<evio::BaseStructure> data_block,
               uint32_t rocid,
               std::vector<PhysicsEvent*>& physics_events,
               TriggerData& trigger_data) override;
    
};

#endif // MODULEPARSER_CAEN1190_H
