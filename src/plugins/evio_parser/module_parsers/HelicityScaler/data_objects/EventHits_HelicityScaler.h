#ifndef _EVENT_HITS_HELICITYSCALER_H_
#define _EVENT_HITS_HELICITYSCALER_H_

#include <JANA/JEvent.h>

#include "EventHits.h"
#include "HelicityScalerHit.h"

/**
 * @class EventHits_HelicityScaler
 * @brief Container for all HelicityScaler detector hits in a single event
 *
 * Owns scaler hit pointers and knows how to insert them into a JEvent.
 */
class EventHits_HelicityScaler : public EventHits {
public:
    std::vector<HelicityScalerHit*> scalers;

    void insertIntoEvent(JEvent& event) override {
        for (auto& scaler : scalers) {
            event.Insert(scaler);
        }
    }
};

#endif // _EVENT_HITS_HELICITYSCALER_H_
