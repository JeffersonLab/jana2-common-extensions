#include <JANA/JApplication.h>

#include "JEventProcessor_DetectorTranslationDump.h"

extern "C" {
    void InitPlugin(JApplication* app) {
        InitJANAPlugin(app);
        app->AddPlugin("detector_translation");
        app->Add(new JEventProcessor_DetectorTranslationDump());
    }
}
