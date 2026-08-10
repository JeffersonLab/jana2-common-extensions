#include <JANA/JApplication.h>

#include "JEventProcessor_DetectorTranslationTest.h"

extern "C" {
    void InitPlugin(JApplication* app) {
        InitJANAPlugin(app);
        app->Add(new JEventProcessor_DetectorTranslationTest());
    }
}
