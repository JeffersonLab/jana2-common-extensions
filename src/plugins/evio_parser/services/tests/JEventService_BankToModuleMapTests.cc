#include <JANA/JException.h>

#include <cassert>

#include "JEventService_BankToModuleMap.h"

int main() {
    JEventService_BankToModuleMap routes;
    routes.addRoute(350, 250);
    routes.addRoute(9250, 9250);
    routes.addRoute(9001, 9001);

    bool duplicate_rejected = false;
    try {
        routes.addRoute(350, 251);
    }
    catch (const JException&) {
        duplicate_rejected = true;
    }
    assert(duplicate_rejected);

    assert(routes.getModuleId(350) == 250);
    assert(routes.getModuleId(9250) == 9250);
    assert(routes.getModuleId(9001) == 9001);
    assert(routes.getModuleId(999) == -1);

    bool frozen_registry_rejected = false;
    try {
        routes.addRoute(351, 251);
    }
    catch (const JException&) {
        frozen_registry_rejected = true;
    }
    assert(frozen_registry_rejected);
}
