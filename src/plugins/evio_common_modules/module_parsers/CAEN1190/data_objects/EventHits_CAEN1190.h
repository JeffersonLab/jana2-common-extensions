#ifndef _EVENT_HITS_CAEN1190_H_
#define _EVENT_HITS_CAEN1190_H_

#include <JANA/JEvent.h>

#include "EventHits.h"
#include "CAEN1190Hit.h"

/**
 * @class EventHits_CAEN1190
 * @brief Container for all CAEN detector hits in a single event
 */
class EventHits_CAEN1190 : public EventHits {
public:
    std::vector<CAEN1190Hit*> caen_hits;

    void insertIntoEvent(JEvent& event) override {
        for (auto& caen_hit : caen_hits) {
            event.Insert(caen_hit);
        }
    }
};

#endif // _EVENT_HITS_CAEN1190_H_
