#include <JANA/JApplication.h>

#include "JEventProcessor_DetectorDigiHits.h"
#include "JEventService_DetectorMappingCatalogs.h"
#include "JEventService_DetectorTranslatorsMap.h"
#include "JEventService_TranslationTable.h"
#include "jce_config_paths.h"

#include <memory>

extern "C" void InitPlugin(JApplication* app) {
    InitJANAPlugin(app);
    app->AddPlugin("evio_parser");
    auto catalogs = std::make_shared<JEventService_DetectorMappingCatalogs>();
    catalogs->addCatalog(
        "jana2_common_extensions",
        jce_config_path(
            "evio_parser/detector_mappings",
            "TRANSLATION:DIRECTORY"));
    app->ProvideService(catalogs);
    app->ProvideService(
        std::make_shared<JEventService_DetectorTranslatorsMap>());
    app->ProvideService(std::make_shared<JEventService_TranslationTable>());
    app->Add(new JEventProcessor_DetectorDigiHits());
}
