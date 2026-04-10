#ifndef _EVENT_HITS_MPD_H_
#define _EVENT_HITS_MPD_H_

#include <JANA/JEvent.h>

#include "EventHits.h"
#include "MPDHit.h"

/**
 * @class EventHits_MPD
 * @brief Container for all MPD hits in a single event
 */
class EventHits_MPD : public EventHits {
public:
    std::vector<MPDHit*> mpd;

    void insertIntoEvent(JEvent& event) override {
        for (auto& item : mpd) {
            event.Insert(item);
        }
    }
};

#endif // _EVENT_HITS_MPD_H_
