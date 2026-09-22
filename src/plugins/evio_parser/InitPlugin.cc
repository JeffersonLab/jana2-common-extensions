
#include <JANA/JApplication.h>           // Core JANA2 application framework
#include <JANA/JEventProcessor.h>
#include <JANA/JEventSourceGeneratorT.h>     // Event source generator template

// Experiment specific components
#include "JEventSource_EVIO.h"              // EVIO file event source
#include "JEventUnfolder_EVIO.h"            // Event unfolder
#include "JEventService_FilterDB.h"         // Service for ROC/bank filtering
#include "JEventService_BankToModuleMap.h"   // Service for mapping bank IDs to module IDs
#include "JEventService_ModuleParsersMap.h"   // Service for mapping module IDs to parser implementations
#include "JEventService_TopLevelEventDecoders.h"

namespace {

// Keeps each event level in the topology active even when an application only
// consumes factories. Experiment plugins may add their own processors normally.
class JEventProcessor_EvioPassthrough final : public JEventProcessor {
public:
    JEventProcessor_EvioPassthrough() {
        SetTypeName("JEventProcessor_EvioPassthrough");
        SetCallbackStyle(CallbackStyle::ExpertMode);
    }

    void ProcessParallel(const JEvent&) override {}
};

} // namespace


extern "C" {
    void InitPlugin(JApplication* app) {
        InitJANAPlugin(app);

        // Register all plugin components
        app->Add(new JEventSourceGeneratorT<JEventSource_EVIO>());
        app->Add(new JEventUnfolder_EVIO());
        app->Add(new JEventProcessor_EvioPassthrough());

        // Register services
        auto filter_svc       = std::make_shared<JEventService_FilterDB>();
        auto bank_to_module_svc = std::make_shared<JEventService_BankToModuleMap>();
        auto module_parsers_svc = std::make_shared<JEventService_ModuleParsersMap>();
        auto event_decoders_svc =
            std::make_shared<JEventService_TopLevelEventDecoders>();

        app->ProvideService(filter_svc);
        app->ProvideService(bank_to_module_svc);
        app->ProvideService(module_parsers_svc);
        app->ProvideService(event_decoders_svc);
    }
}
