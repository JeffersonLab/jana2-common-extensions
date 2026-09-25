#include <JANA/JApplication.h>
#include <JANA/JService.h>
#include <memory>
#include "JEventService_ModuleParsersMap.h"

// Module parsers
#include "ModuleParser_CAEN1190.h"
#include "ModuleParser_FADC.h"
#include "ModuleParser_FADCScaler.h"
#include "ModuleParser_TIScaler.h"
#include "ModuleParser_HelicityDecoder.h"
#include "ModuleParser_MPD.h"
#include "ModuleParser_VFTDC.h"

class JEventService_CommonModuleParsers final : public JService {
public:
    Service<JEventService_ModuleParsersMap> parsers {this};

    void Init() override {
        add(1190, std::make_shared<ModuleParser_CAEN1190>());
        add(250, std::make_shared<ModuleParser_FADC>());
        add(9250, std::make_shared<ModuleParser_FADCScaler>());
        add(9001, std::make_shared<ModuleParser_TIScaler>());
        add(0xdec, std::make_shared<ModuleParser_HelicityDecoder>());
        add(3561, std::make_shared<ModuleParser_MPD>());
        add(9, std::make_shared<ModuleParser_VFTDC>());
    }

private:
    void add(int id, std::shared_ptr<BankParser> parser) {
        parsers->addParser(id, std::move(parser));
    }
};

void RegisterCommonModuleParsers(JApplication* app) {
    app->ProvideService(std::make_shared<JEventService_CommonModuleParsers>());
}
