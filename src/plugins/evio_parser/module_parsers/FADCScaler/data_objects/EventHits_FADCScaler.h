#ifndef _EVENT_HITS_FADCSCALER_H_
#define _EVENT_HITS_FADCSCALER_H_

#include <JANA/JEvent.h>

#include "EventHits.h"
#include "FADCScalerHit.h"

/**
 * @class EventHits_FADCScaler
 * @brief Container for all FADCScaler detector hits in a single event
 *
 * Owns scaler hit pointers and knows how to insert them into a JEvent.
 */
class EventHits_FADCScaler : public EventHits {
public:
    std::vector<FADCScalerHit*> scalers;

    void insertIntoEvent(JEvent& event) override {
        for (auto& scaler : scalers) {
            event.Insert(scaler);
        }
    }
};

#endif // _EVENT_HITS_FADCSCALER_H_
