
#include <JANA/JApplication.h>           // Core JANA2 application framework
#include <JANA/JEventSourceGeneratorT.h>     // Event source generator template

// Experiment specific components
#include "JEventSource_EVIO.h"              // EVIO file event source
#include "JEventUnfolder_EVIO.h"            // Event unfolder
#include "JEventService_FilterDB.h"         // Service for ROC/bank filtering
#include "JEventService_BankToModuleMap.h"   // Service for mapping bank IDs to module IDs
#include "JEventService_ModuleParsersMap.h"   // Service for mapping module IDs to parser implementations
#include "JEventService_DetectorTranslatorsMap.h" // Detector translator registry
#include "TranslationTableService.h"          // Run-specific detector translation tables
#include "JEventProcessor_DetectorDigiHits.h"  // Central detector DigiHit routing

// Module parsers registration function
void InitModuleParsers(JApplication* app);
void InitDetectorTranslators(JApplication* app);


extern "C" {
    void InitPlugin(JApplication* app) {
        InitJANAPlugin(app);

        // Register all plugin components
        app->Add(new JEventSourceGeneratorT<JEventSource_EVIO>());
        app->Add(new JEventUnfolder_EVIO());
        app->Add(new JEventProcessor_DetectorDigiHits());

        // Register services
        auto filter_svc       = std::make_shared<JEventService_FilterDB>();
        auto bank_to_module_svc = std::make_shared<JEventService_BankToModuleMap>();
        auto module_parsers_svc = std::make_shared<JEventService_ModuleParsersMap>();
        auto detector_translators_svc =
            std::make_shared<JEventService_DetectorTranslatorsMap>();
        auto translation_table_svc = std::make_shared<TranslationTableService>();

        app->ProvideService(filter_svc);
        app->ProvideService(bank_to_module_svc);
        app->ProvideService(module_parsers_svc);
        app->ProvideService(detector_translators_svc);
        app->ProvideService(translation_table_svc);

        // Initialize module parsers
        InitModuleParsers(app);
        InitDetectorTranslators(app);
    }
}
