#ifndef _TRIGGER_DATA_H_
#define _TRIGGER_DATA_H_

#include <cstdint>

/**
 * @class TriggerData
 * @brief Simple POD type holding the first event metadata for an EVIO block
 */
class TriggerData {
public:
   uint64_t first_event_number;
   uint32_t last_event_number;

   TriggerData()
      : first_event_number(0), last_event_number(0) {}

   TriggerData(uint64_t first_event_number)
      : first_event_number(first_event_number), last_event_number(0) {}
};

#endif // _TRIGGER_DATA_H_
