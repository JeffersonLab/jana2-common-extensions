#include <JANA/JApplication.h>

#include "FADC250PulseHit.h"
#include "HMSHodoscopeTranslator.h"
#include "JEventService_DetectorTranslatorsMap.h"

void InitDetectorTranslators(JApplication* app) {
    auto translators =
        app->GetService<JEventService_DetectorTranslatorsMap>();

    translators->addTranslator<FADC250PulseHit>(
        "HMS_HODOSCOPE",
        translateHMSHodoscopeFADCHit);

    translators->freeze();
}
