

#ifndef _JEventSource_EVIO_h_
#define  _JEventSource_EVIO_h_

#include <JANA/JEventSource.h>
#include <JANA/JEventSourceGeneratorT.h>

#include <fstream>
#include <filesystem>

#include "eviocc.h"
#include "JEventService_TopLevelEventDecoders.h"


/**
 * @class JEventSource_EVIO
 * @brief JANA2 event source for reading EVIO format data files
 * 
 * This class handles the reading of EVIO files and creation of events
 * for the experiment data processing pipeline.
 */
class JEventSource_EVIO : public JEventSource {

private:
    std::unique_ptr<evio::EvioReader> m_evio_reader;  ///< EVIO file reader instance
    std::shared_ptr<evio::EvioEvent> m_pending_event;
    std::shared_ptr<JEventService_TopLevelEventDecoders> m_user_decoders;
    int m_run_number = 0;                             ///< Current run number

    /**
     * @brief Identifies run control events and extracts run number from prestart events
     *
     * Run control events have tags in the range 0xFFD0-0xFFDF. When a prestart event
     * (tag 0xFFD1) is detected, the run number is extracted from the event data and
     * stored in the run_number parameter.
     *
     * @param event       EVIO event to examine
     * @param run_number  Reference to run number (updated if prestart event found)
     * @return true if this was any run control event, false otherwise
     */
    static bool isRunControlEvent(std::shared_ptr<evio::EvioEvent> event, int& run_number);

    /**
     * @brief Identifies physics events by their EVIO tag
     *
     * @param event  EVIO event to examine
     * @return true if this was a physics event (tag 0xFF50 or 0xFF58), false otherwise
     */
    static bool isPhysicsEvent(std::shared_ptr<evio::EvioEvent> event);

public:
    JEventSource_EVIO();
    virtual ~JEventSource_EVIO() = default;
    
    void Open() override;
    void Close() override;
    Result Emit(JEvent& event) override;
    /// Static description is required by JEventSourceGeneratorT<T>
    static std::string GetDescription();

};

template <>
double JEventSourceGeneratorT<JEventSource_EVIO>::CheckOpenable(std::string);

#endif // _JEventSource_EVIO_h_
