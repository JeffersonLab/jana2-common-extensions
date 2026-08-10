
#include <JANA/JApplication.h>           // Core JANA2 application framework
#include <JANA/JEventSourceGeneratorT.h>     // Event source generator template
#include <JANA/JFactoryGenerator.h>          // Lightweight-data-model factory generator
#include <JANA/JEventProcessor.h>            // Event processor

// Experiment specific components
#include "JEventSource_EVIO.h"              // EVIO file event source
#include "JEventUnfolder_EVIO.h"            // Event unfolder
#include "JEventService_FilterDB.h"         // Service for ROC/bank filtering
#include "JEventService_BankToModuleMap.h"   // Service for mapping bank IDs to module IDs
#include "JEventService_ModuleParsersMap.h"   // Service for mapping module IDs to parser implementations
#include "TranslationTableService.h"          // Run-specific detector translation tables
#include "DetectorDigiHit_factory.h"           // Central detector DigiHit routing

// Module parsers registration function
void InitModuleParsers(JApplication* app);


// Satisfies the topology requirement for an unfolder: at least one processor
// must exist downstream so JANA can wire a complete source→unfolder→processor
// chain. This processor does nothing; any real processor plugin loaded alongside
// evio_source will take precedence and this one simply passes through.
struct JEventProcessor_Passthrough : public JEventProcessor {
    JEventProcessor_Passthrough() { SetTypeName("JEventProcessor_Passthrough"); }
    void ProcessSequential(const JEvent&) override {}
};

extern "C" {
    void InitPlugin(JApplication* app) {
        InitJANAPlugin(app);

        // Register all plugin components
        app->Add(new JEventSourceGeneratorT<JEventSource_EVIO>());
        app->Add(new JEventUnfolder_EVIO());
        app->Add(new JEventProcessor_Passthrough());
        app->Add(new JFactoryGeneratorT<DetectorDigiHit_factory>());

        // Register services
        auto filter_svc       = std::make_shared<JEventService_FilterDB>();
        auto bank_to_module_svc = std::make_shared<JEventService_BankToModuleMap>();
        auto module_parsers_svc = std::make_shared<JEventService_ModuleParsersMap>();
        auto translation_table_svc = std::make_shared<TranslationTableService>();

        app->ProvideService(filter_svc);
        app->ProvideService(bank_to_module_svc);
        app->ProvideService(module_parsers_svc);
        app->ProvideService(translation_table_svc);

        // Initialize module parsers
        InitModuleParsers(app);
    }
}
