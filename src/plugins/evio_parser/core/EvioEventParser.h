#ifndef EVIOEVENTPARSER_H
#define EVIOEVENTPARSER_H

#include <string>
#include <vector>
#include <memory>

#include <JANA/JApplication.h>
#include <JANA/JLogger.h> 
#include <JANA/JEvent.h>

#include "eviocc.h"
#include "BankParser.h"
#include "EventHits.h"
#include "PhysicsEvent.h"
#include "TriggerData.h"

/**
 * @class EvioEventParser
 * @brief Helper class for processing a single EVIO block-level JEvent
 *
 * An instance of this class is constructed with a pointer to the JApplication
 * (so it can access services such as JEventService_BankToModuleMap and
 * JEventService_ModuleParsersMap). The actual
 * JEvent to be parsed is passed to the parse(const JEvent&) method, which
 * extracts PhysicsEvent objects from the EVIO data.
 */
class EvioEventParser {
private:
    
    JApplication* m_app;        ///< Owning JApplication (for accessing services)
    JLogger& m_logger;         ///< Reference to the jana::JLogger
    /**
     * Parse trigger bank and extract ROC segments
     *
     * @param trigger_bank  The trigger bank to process
     * @param trigger_data  Output trigger metadata (first event number, etc.)
     * @return Vector of trigger bank ROC segments
     */
    std::vector<std::shared_ptr<evio::BaseStructure>> 
    parseTriggerBank(std::shared_ptr<evio::BaseStructure> trigger_bank,
                     TriggerData& trigger_data);
    
    /**
     * Parse ROC banks and extract hits
     *
     * @param roc_banks                  Vector of ROC banks
     * @param trigger_bank_roc_segments  Vector of trigger bank ROC segments
     * @param trigger_data               Trigger metadata for this EVIO block
     * @param physics_events             Output vector which will be filled with PhysicsEvent*
     */
    void parseROCBanks(const std::vector<std::shared_ptr<evio::BaseStructure>>& roc_banks,
                       const std::vector<std::shared_ptr<evio::BaseStructure>>& trigger_bank_roc_segments,
                       TriggerData& trigger_data,
                       std::vector<PhysicsEvent*>& physics_events);

public:
    /**
     * @brief Constructor
     *
     * Stores the application pointer for later use. The event to parse is
     * provided when calling parse(const JEvent&).
     *
     * @param app Pointer to the owning JApplication
     * @param logger Reference to the jana::JLogger
     */
    EvioEventParser(JApplication* app, JLogger& logger)
        : m_app(app), m_logger(logger) {};
    
    /**
     * Destructor
     */
    ~EvioEventParser() = default;
    
    /**
     * @brief Parse the EVIO event and extract all hits
     *
     * This method inspects the given block-level JEvent, pulls out the
     * underlying EVIO event (via EvioEventWrapper), and:
     *  1. Parses the trigger bank to obtain trigger metadata and ROC segments
     *  2. Resolves each bank ID to a module ID, then parses using the registered module parsers
     *  3. Fills the provided vector of PhysicsEvent* representing physics events
     *
     * @param event           Block-level JEvent containing an EvioEventWrapper
     * @param physics_events  Output vector which will be filled with PhysicsEvent*
     */
    void parse(const JEvent& event, std::vector<PhysicsEvent*>& physics_events);

    /**
     * @brief Get the logger
     *
     * @return Reference to the jana::JLogger
     */
    JLogger& GetLogger() { return m_logger; }
};

#endif // EVIOEVENTPARSER_H
