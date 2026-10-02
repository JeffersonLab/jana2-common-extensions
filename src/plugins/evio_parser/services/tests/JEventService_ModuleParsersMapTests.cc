#include <JANA/JException.h>

#include <cassert>
#include <memory>

#include "JEventService_ModuleParsersMap.h"

namespace {

class TestModuleParser final : public ModuleParser {};

} // namespace

int main() {
    JEventService_ModuleParsersMap parsers;
    auto parser = std::make_shared<TestModuleParser>();
    parsers.addParser(250, parser);

    bool null_rejected = false;
    try {
        parsers.addParser(251, nullptr);
    }
    catch (const JException&) {
        null_rejected = true;
    }
    assert(null_rejected);

    bool duplicate_rejected = false;
    try {
        parsers.addParser(250, std::make_shared<TestModuleParser>());
    }
    catch (const JException&) {
        duplicate_rejected = true;
    }
    assert(duplicate_rejected);

    assert(parsers.getParser(250) == parser);
    assert(parsers.getParser(999) == nullptr);

    bool frozen_registry_rejected = false;
    try {
        parsers.addParser(252, std::make_shared<TestModuleParser>());
    }
    catch (const JException&) {
        frozen_registry_rejected = true;
    }
    assert(frozen_registry_rejected);
}
