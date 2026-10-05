#include <JANA/JApplication.h>

#include "InitHMSHodoscopeTranslators.h"
#include "JEventService_DetectorMappingCatalogs.h"
#include "JEventService_DetectorTranslatorsMap.h"
#include "jce_config_paths.h"

extern "C" void InitPlugin(JApplication* app) {
    InitJANAPlugin(app);
    app->AddPlugin("evio_common_modules");
    app->AddPlugin("detector_translation");
    app->GetService<JEventService_DetectorMappingCatalogs>()->addCatalog(
        "hms_detector_translation",
        jce_config_path(
            "hms_detector_translation/detector_mappings",
            "TRANSLATION:DIRECTORY"));
    InitHMSHodoscopeTranslators(
        *app->GetService<JEventService_DetectorTranslatorsMap>());
}
