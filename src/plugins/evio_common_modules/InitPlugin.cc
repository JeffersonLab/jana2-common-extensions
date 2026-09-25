#include <JANA/JApplication.h>
#include <JANA/JService.h>

#include <memory>

void RegisterCommonModuleParsers(JApplication* app);

extern "C" void InitPlugin(JApplication* app) {
    InitJANAPlugin(app);
    app->AddPlugin("evio_parser");
    RegisterCommonModuleParsers(app);
}
