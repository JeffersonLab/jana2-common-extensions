#ifndef _EVENT_HITS_VFTDC_H_
#define _EVENT_HITS_VFTDC_H_

#include <JANA/JEvent.h>

#include "EventHits.h"
#include "VFTDCHit.h"

/**
 * @class EventHits_VFTDC
 * @brief Container for all VFTDC hits in a single event
 */
class EventHits_VFTDC : public EventHits {
public:
    std::vector<VFTDCHit*> vftdc_hits;

    void insertIntoEvent(JEvent& event) override {
        for (auto& vftdc_hit : vftdc_hits) {
            event.Insert(vftdc_hit);
        }
    }
};

#endif // _EVENT_HITS_VFTDC_H_
