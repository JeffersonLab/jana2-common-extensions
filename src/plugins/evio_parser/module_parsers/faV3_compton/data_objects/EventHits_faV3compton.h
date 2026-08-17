#ifndef _EVENT_HITS_FAV3COMPTON_H_
#define _EVENT_HITS_FAV3COMPTON_H_

#include <JANA/JEvent.h>

#include "EventHits.h"
#include "faV3comptonHit.h"

/**
 * @class EventHits_faV3compton
 * @brief Container for all compton FADC detector hits in a single event
 *
 * Owns waveform and pulse hit pointers and knows how to insert
 * them into a JEvent.
 */
class EventHits_faV3compton : public EventHits {
public:

    void insertIntoEvent(JEvent& event) override {

    }
};

#endif // _EVENT_HITS_FAV3COMPTON_H_
