#include <JANA/JApplication.h>

void InitModuleParsers(JApplication* app);

extern "C" void InitPlugin(JApplication* app) {
    InitJANAPlugin(app);
    app->AddPlugin("evio_parser");
    InitModuleParsers(app);
}
