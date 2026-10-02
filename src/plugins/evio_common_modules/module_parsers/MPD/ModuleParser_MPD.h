#ifndef MODULEPARSER_MPD_H
#define MODULEPARSER_MPD_H

#include "ModuleParser.h"
#include "MPDHit.h"
#include "EventHits_MPD.h"
/**
 * @class ModuleParser_MPD
 * @brief ModuleParser implementation for MPD data
 */
class ModuleParser_MPD : public ModuleParser {
public:
    void parse(std::shared_ptr<evio::BaseStructure> data_block,
               uint32_t rocid,
               std::vector<PhysicsEvent*>& physics_events,
               TriggerData& trigger_data) override;
    
};

#endif // MODULEPARSER_MPD_H