
#include "JEventSource_EVIO.h"
#include "EvioEventWrapper.h"

#include <JANA/JApplication.h>
#include <JANA/JEvent.h>


/// Include headers to any JObjects you wish to associate with each event
// #include "Hit.h"

/// There are two different ways of instantiating JEventSources
/// 1. Creating them manually and registering them with the JApplication
/// 2. Creating a corresponding JEventSourceGenerator and registering that instead
///    If you have a list of files as command line args, JANA will use the JEventSourceGenerator
///    to find the most appropriate JEventSource corresponding to that filename, instantiate and register it.
///    For this to work, the JEventSource constructor has to have the following constructor arguments:


/**
 * @brief Constructor for JEventSource_EVIO
 * 
 * Initializes the event source with the appropriate type name and callback style.
 */
JEventSource_EVIO::JEventSource_EVIO() : JEventSource() {
    SetTypeName(NAME_OF_THIS);                    // Provide JANA with class name
    SetPrefix("EVIO_PARSER");
    SetCallbackStyle(CallbackStyle::ExpertMode);
    SetLevel(JEventLevel::Block);
    SetParentLevels({JEventLevel::Run, JEventLevel::SlowControls});
    SetNextEventLevel(JEventLevel::Block);
}

/**
 * @brief Open the input source
 * Called once at the beginning of processing
 */
void JEventSource_EVIO::Open() {

    /// Get any configuration parameters from the JApplication
    // GetApplication()->SetDefaultParameter("JEventSource_EVIO:random_seed", m_seed, "Random seed");
    
    // For opening a file, get the filename via:
    const std::string resource_name = GetResourceName();
    /// Open the file here!
    m_evio_reader = std::make_unique<evio::EvioReader>(resource_name);
    m_user_decoders =
        GetApplication()->GetService<JEventService_TopLevelEventDecoders>();

}

/**
 * @brief Finish processing and cleanup
 * 
 * Called once at the end of processing to perform cleanup. This is where you should close your
  files or sockets. It is important to do that here instead of in Emit() because we want
 * everything to be cleanly closed even when JANA is terminated via Ctrl-C or via a timeout.
 */
void JEventSource_EVIO::Close() {
    // Close the EVIO file reader
    m_evio_reader->close();
}

/**
 * @brief Emit the next event
 * 
 * 
 * @param event Reference to the JANA2 event to populate
 * @return Result indicating success, failure, or end of file
 */
JEventSource::Result JEventSource_EVIO::Emit(JEvent& event) {
    if (m_pending_event == nullptr) {
        m_pending_event = m_evio_reader->parseNextEvent();
    }

    // Check for end of file
    if (m_pending_event == nullptr) {
        return Result::FailureFinished;
    }

    const auto header = m_pending_event->getHeader();
    const auto tag = static_cast<std::uint16_t>(header->getTag());
    const auto number = static_cast<std::uint8_t>(header->getNumber());
    const auto data_type = static_cast<std::uint8_t>(
        header->getDataType().getValue());

    EvioEventKind kind;
    JEventLevel level;
    std::string decoder_key;

    if (isRunControlEvent(m_pending_event, m_run_number)) {
        kind = EvioEventKind::Control;
        level = JEventLevel::Run;
    } else if (isPhysicsEvent(m_pending_event)) {
        kind = EvioEventKind::Physics;
        level = JEventLevel::Block;
    } else {
        auto match = m_user_decoders->resolve(tag, number, data_type);
        if (!match) {
            LOG_DEBUG(GetLogger())
                << "Skipping unregistered EVIO event tag 0x"
                << std::hex << tag << std::dec << LOG_END;
            m_pending_event.reset();
            return Result::FailureTryAgain;
        }
        kind = EvioEventKind::User;
        level = match->level;
        decoder_key = match->key;
    }

    if (event.GetLevel() != level) {
        SetNextEventLevel(level);
        return Result::FailureLevelChange;
    }

    event.SetRunNumber(m_run_number);
    event.SetEventNumber(m_pending_event->getEventNumber());
    event.Insert(new EvioEventWrapper(
        m_pending_event, kind, std::move(decoder_key)));
    if (kind == EvioEventKind::User) {
        const auto* wrapper = event.GetSingle<EvioEventWrapper>();
        auto match = m_user_decoders->resolve(tag, number, data_type);
        TopLevelEventContext context {
            tag, number, data_type, m_pending_event->getEventNumber(),
            event.GetRunNumber(), GetLogger()
        };
        match->decoder->decode(wrapper->evio_event, context, event);
    }
    m_pending_event.reset();

    // JMultilevelSourceArrow requests its next pool by this level. Keep the
    // following event buffered so it never requests a parent pool too early.
    while ((m_pending_event = m_evio_reader->parseNextEvent()) != nullptr) {
        const auto next_header = m_pending_event->getHeader();
        const auto next_tag = static_cast<std::uint16_t>(next_header->getTag());
        if (next_tag >= 0xFFD0 && next_tag <= 0xFFDF) {
            SetNextEventLevel(JEventLevel::Run);
            break;
        }
        if (isPhysicsEvent(m_pending_event)) {
            SetNextEventLevel(JEventLevel::Block);
            break;
        }
        auto next_match = m_user_decoders->resolve(
            next_tag,
            static_cast<std::uint8_t>(next_header->getNumber()),
            static_cast<std::uint8_t>(next_header->getDataType().getValue()));
        if (next_match) {
            SetNextEventLevel(next_match->level);
            break;
        }
        LOG_DEBUG(GetLogger()) << "Skipping unregistered EVIO event tag 0x"
            << std::hex << next_tag << std::dec << LOG_END;
    }
    if (level != JEventLevel::Block) {
        // This JANA multilevel arrow evicts the previous parent only when
        // its next requested level is another parent of the same kind.
        SetNextEventLevel(level);
    } else if (m_pending_event == nullptr) {
        SetNextEventLevel(JEventLevel::Block);
    }
    return Result::Success;
}

/**
 * @brief Get description of this event source
 * @return Description string
 */
std::string JEventSource_EVIO::GetDescription() {
    return "EVIO event source for experiment data";
}

/**
 * @brief Identifies physics events by their EVIO tag
 *
 * Physics events are identified by tags 0xFF50 and 0xFF58.
 *
 * @param event       EVIO event to examine
 * @return true if this was a physics event (tag 0xFF50 or 0xFF58), false otherwise
 */
bool JEventSource_EVIO::isPhysicsEvent(std::shared_ptr<evio::EvioEvent> event) {
    std::shared_ptr<evio::BaseStructureHeader> header = event->getHeader();
    uint16_t tag = header->getTag();
    if (tag == 0xFF50 || tag == 0xFF58 ||
        tag == 0xFF70 || tag == 0xFF78) {
        return true;
    }
    
    return false; 
}

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
bool JEventSource_EVIO::isRunControlEvent(std::shared_ptr<evio::EvioEvent> event, int& run_number) {
    std::shared_ptr<evio::BaseStructureHeader> header = event->getHeader();
    uint16_t tag = header->getTag();
    
    if (tag >= 0xFFD0 && tag <= 0xFFDF) {
        if (tag == 0xFFD1) { // prestart event
            std::vector<uint32_t> data = event->getUIntData();
            if (data.size() > 1) {
                run_number = data[1];  // Run number is stored at index 1
            } else {
                throw JException("Prestart event has no data");
            }
        }
        return true;
    }
    
    return false; 
}


/**
 * @brief Check if this event source can handle a given file
 * 
 * Validates that the specified file is a valid EVIO file by attempting to open
 * it with EvioReader.
 * 
 * The function is called by JANA2's event source generator system to determine
 * which event source should handle a given file. A higher confidence value
 * indicates a better match.
 * 
 * @param resource_name Name/path of the resource (file) to check
 * @return Confidence level:
 *         - 1.0 if the file can be opened and is a valid EVIO file (high confidence)
 *         - 0.0 if the file cannot be opened or is not a valid EVIO file (cannot handle)
 */
template <>
double JEventSourceGeneratorT<JEventSource_EVIO>::CheckOpenable(std::string resource_name) {
    
    try {
        // Attempt to create EvioReader to open and validate the file
        // This will throw an exception if the file is not a valid EVIO file,
        // cannot be opened, or has an invalid format structure
        evio::EvioReader reader(resource_name);
        
        // If we reach here, the file was successfully opened and validated as EVIO
        return 1.0;
    } 
    catch (const std::exception& e) {
        // File validation failed - either not an EVIO file or cannot be opened
        return 0.0;
    }
}
