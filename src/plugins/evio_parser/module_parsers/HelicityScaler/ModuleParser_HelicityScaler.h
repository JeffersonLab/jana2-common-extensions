#ifndef MODULEPARSER_HELICITYSCALER_H
#define MODULEPARSER_HELICITYSCALER_H

#include "ModuleParser.h"
#include "HelicityScalerHit.h"
#include "EventHits_HelicityScaler.h"

/**
 * @class ModuleParser_HelicityScaler
 * @brief ModuleParser implementation for helicity scaler data
 *
 * This parser decodes scaler words from an EVIO bank and fills
 * an EventHits_HelicityScaler container with HelicityScalerHit objects.
 */
class ModuleParser_HelicityScaler : public ModuleParser {
public:
    void parse(std::shared_ptr<evio::BaseStructure> data_block,
               uint32_t rocid,
               std::vector<PhysicsEvent*>& physics_events,
               TriggerData& block_first_event_data) override;
};

#endif // MODULEPARSER_HELICITYSCALER_H
