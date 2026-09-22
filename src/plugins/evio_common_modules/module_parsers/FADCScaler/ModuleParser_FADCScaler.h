#ifndef MODULEPARSER_FADCSCALER_H
#define MODULEPARSER_FADCSCALER_H

#include "ModuleParser.h"
#include "FADCScalerHit.h"
#include "EventHits_FADCScaler.h"

/**
 * @class ModuleParser_FADCScaler
 * @brief ModuleParser implementation for FADC scaler data
 *
 * This parser decodes scaler words from an EVIO bank and fills
 * an EventHits_FADCScaler container with FADCScalerHit objects.
 */
class ModuleParser_FADCScaler : public ModuleParser {
public:
    void parse(std::shared_ptr<evio::BaseStructure> data_block,
               uint32_t rocid,
               std::vector<PhysicsEvent*>& physics_events,
               TriggerData& block_first_event_data) override;
};

#endif // MODULEPARSER_FADCSCALER_H