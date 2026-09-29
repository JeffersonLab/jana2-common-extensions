#include <JANA/JApplication.h>
#include <JANA/JService.h>

#include "InitHMSHodoscopeTranslators.h"
#include "JEventService_DetectorTranslatorsMap.h"

#include <memory>

namespace {

class JEventService_HMSTranslatorRegistration final : public JService {
public:
    Service<JEventService_DetectorTranslatorsMap> translators {this};

    void Init() override {
        InitHMSHodoscopeTranslators(translators());
    }
};

} // namespace

extern "C" void InitPlugin(JApplication* app) {
    InitJANAPlugin(app);
    app->AddPlugin("evio_common_modules");
    app->AddPlugin("detector_translation");
    app->ProvideService(
        std::make_shared<JEventService_HMSTranslatorRegistration>());
}
