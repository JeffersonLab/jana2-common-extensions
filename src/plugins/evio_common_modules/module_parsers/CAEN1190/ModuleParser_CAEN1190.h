#ifndef MODULEPARSER_CAEN1190_H
#define MODULEPARSER_CAEN1190_H

#include "ModuleParser.h"
#include "CAEN1190Hit.h"
#include "EventHits_CAEN1190.h"
/**
 * @class ModuleParser_CAEN1190
 * @brief ModuleParser implementation for CAEN1190 data
 */
class ModuleParser_CAEN1190 : public ModuleParser {
public:
    void parse(std::shared_ptr<evio::BaseStructure> data_block,
               uint32_t rocid,
               std::vector<PhysicsEvent*>& physics_events,
               TriggerData& trigger_data) override;
    
};

#endif // MODULEPARSER_CAEN1190_H
