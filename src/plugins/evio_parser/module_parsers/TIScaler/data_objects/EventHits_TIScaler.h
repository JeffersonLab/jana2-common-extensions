#ifndef _EVENT_HITS_TISCALER_H_
#define _EVENT_HITS_TISCALER_H_

#include <JANA/JEvent.h>

#include "EventHits.h"
#include "TIScalerHit.h"

/**
 * @class EventHits_TIScaler
 * @brief Container for all TIScaler detector hits in a single event
 *
 * Owns scaler hit pointers and knows how to insert them into a JEvent.
 */
class EventHits_TIScaler : public EventHits {
public:
    std::vector<TIScalerHit*> scalers;

    void insertIntoEvent(JEvent& event) override {
        for (auto& scaler : scalers) {
            event.Insert(scaler);
        }
    }
};

#endif // _EVENT_HITS_TISCALER_H_

