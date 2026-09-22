#include <JANA/JApplication.h>

#include "InitHMSHodoscopeTranslators.h"
#include "JEventService_DetectorTranslatorsMap.h"

void InitDetectorTranslators(JApplication* app) {
    auto translators =
        app->GetService<JEventService_DetectorTranslatorsMap>();

    InitHMSHodoscopeTranslators(*translators);

    translators->freeze();
}
