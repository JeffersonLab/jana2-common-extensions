#include <JANA/JApplication.h>

#include "JEventProcessor_DetectorDigiHits.h"
#include "JEventService_DetectorTranslatorsMap.h"
#include "JEventService_TranslationTable.h"

#include <memory>

extern "C" void InitPlugin(JApplication* app) {
    InitJANAPlugin(app);
    app->AddPlugin("evio_parser");

    app->ProvideService(
        std::make_shared<JEventService_DetectorTranslatorsMap>());
    app->ProvideService(std::make_shared<JEventService_TranslationTable>());
    app->Add(new JEventProcessor_DetectorDigiHits());
}
