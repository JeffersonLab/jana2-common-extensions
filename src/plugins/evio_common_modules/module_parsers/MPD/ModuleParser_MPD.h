#ifndef MODULEPARSER_MPD_H
#define MODULEPARSER_MPD_H

#include "BankParser.h"
#include "MPDHit.h"
#include "EventHits_MPD.h"
/**
 * @class ModuleParser_MPD
 * @brief BankParser implementation for MPD data
 */
class ModuleParser_MPD : public BankParser {
public:
    void parse(std::shared_ptr<evio::BaseStructure> data_block,
               uint32_t rocid,
               std::vector<PhysicsEvent*>& physics_events,
               TriggerData& trigger_data) override;
    
};

#endif // MODULEPARSER_MPD_H
