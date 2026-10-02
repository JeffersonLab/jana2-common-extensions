#ifndef MODULEPARSER_TISCALER_H
#define MODULEPARSER_TISCALER_H

#include "ModuleParser.h"
#include "TIScalerHit.h"
#include "EventHits_TIScaler.h"

/**
 * @class ModuleParser_TIScaler
 * @brief ModuleParser implementation for TI scaler data
 *
 * This parser decodes TI scaler words from an EVIO bank and fills
 * an EventHits_TIScaler container with TIScalerHit objects.
 */
class ModuleParser_TIScaler : public ModuleParser {
public:
    void parse(std::shared_ptr<evio::BaseStructure> data_block,
               uint32_t rocid,
               std::vector<PhysicsEvent*>& physics_events,
               TriggerData& trigger_data) override;
};

#endif // MODULEPARSER_TISCALER_H

