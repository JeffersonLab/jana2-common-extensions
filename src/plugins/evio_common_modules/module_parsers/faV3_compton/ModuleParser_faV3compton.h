#ifndef MODULEPARSER_faV3compton_H
#define MODULEPARSER_faV3compton_H

#include "ModuleParser.h"

/**
 * @class ModuleParser_faV3compton
 * @brief ModuleParser implementation for faV3compton250 data
 */
class ModuleParser_faV3compton : public ModuleParser {
public:
    void parse(std::shared_ptr<evio::BaseStructure> data_block,
               uint32_t rocid,
               std::vector<PhysicsEvent*>& physics_events,
               TriggerData& trigger_data) override;


};

#endif // MODULEPARSER_faV3compton_H
