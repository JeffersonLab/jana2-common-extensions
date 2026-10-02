#include <JANA/JApplication.h>

void RegisterCommonModuleParsers(JApplication* app);

extern "C" void InitPlugin(JApplication* app) {
    InitJANAPlugin(app);
    app->AddPlugin("evio_parser");
    RegisterCommonModuleParsers(app);
}
