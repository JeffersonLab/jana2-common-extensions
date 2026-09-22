

#ifndef _EvioEventWrapper_h_
#define _EvioEventWrapper_h_

#include <JANA/JObject.h>

#include <string>
#include <utility>

#include "eviocc.h"

enum class EvioEventKind {
    Physics,
    Control,
    User
};

/**
 * @class EvioEventWrapper
 * @brief JANA2 object wrapper for EVIO events
 * 
 * This class wraps EVIO events as JANA2 objects, allowing them to be passed
 * through the JANA2 event processing pipeline. It maintains a shared pointer
 * to the original EVIO event.
 * 
 * JObjects are plain-old data containers for inputs, intermediate results, and outputs.
 * They have member functions for introspection and maintaining associations with other JObjects, but
 * all of the numerical code which goes into their creation should live in a JFactory instead.
 * You are allowed to include STL containers and pointers to non-POD datatypes inside your JObjects,
 * however, it is highly encouraged to keep them flat and include only primitive datatypes if possible.
 * Think of a JObject as being a row in a database table, with event number as an implicit foreign key.
 */
class EvioEventWrapper : public JObject {

public:

    JOBJECT_PUBLIC(EvioEventWrapper)

    std::shared_ptr<evio::EvioEvent> evio_event;
    EvioEventKind kind = EvioEventKind::Physics;
    std::string decoder_key;

    /**
     * @brief Constructor
     * @param evio_event Shared pointer to the EVIO event to wrap
     */
    explicit EvioEventWrapper(
        std::shared_ptr<evio::EvioEvent> event,
        EvioEventKind event_kind = EvioEventKind::Physics,
        std::string key = {})
        : evio_event(std::move(event)),
          kind(event_kind),
          decoder_key(std::move(key)) {}

    /**
     * @brief Provide string representation of EvioEventWrapper
     * 
     * This method tells JANA how to produce a convenient string representation
     * for this JObject. This can be called from user code, but also lets JANA
     * automatically inspect its own data. See the CsvWriter example.
     * 
     * Warning: Because this is slow, it should be used for debugging and monitoring 
     * but not inside the performance critical code paths.
     * 
     * @param summary Reference to the JObjectSummary to populate
     */
    void Summarize(JObjectSummary& summary) const override {
        std::string eventStr = evio_event->toString();
        summary.add(eventStr, "evio_event", "%s", "EVIO event summary");
    }
};

#endif // _EvioEventWrapper_h_
