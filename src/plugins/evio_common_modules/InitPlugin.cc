#include <JANA/JApplication.h>
#include <JANA/JService.h>

#include <memory>

std::shared_ptr<JService> MakeCommonModuleParserRegistrationService();

extern "C" void InitPlugin(JApplication* app) {
    InitJANAPlugin(app);
    app->AddPlugin("evio_parser");
    app->ProvideService(MakeCommonModuleParserRegistrationService());
}
