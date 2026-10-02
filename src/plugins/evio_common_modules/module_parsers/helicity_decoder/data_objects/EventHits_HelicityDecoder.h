#ifndef _EVENT_HITS_HELICITYDECODER_H_
#define _EVENT_HITS_HELICITYDECODER_H_

#include <JANA/JEvent.h>

#include "EventHits.h"
#include "HelicityDecoderData.h"

/**
 * @class EventHits_HelicityDecoder
 * @brief Container for all HelicityDecoderData detector hits in a single event
 *
 * Owns helicity decoder hit pointers and knows how to insert them into a JEvent.
 */
class EventHits_HelicityDecoder : public EventHits {
public:
    std::vector<HelicityDecoderData*> helicity;

    void insertIntoEvent(JEvent& event) override {
        for (auto& item : helicity) {
            event.Insert(item);
        }
    }
};

#endif // _EVENT_HITS_HELICITYDECODER_H_
