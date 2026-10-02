#include <JANA/JException.h>

#include <cassert>

#include "JEventService_DetectorMappingCatalogs.h"

int main() {
    JEventService_DetectorMappingCatalogs catalogs;
    catalogs.addCatalog("hms", "/config/hms");

    bool duplicate_rejected = false;
    try {
        catalogs.addCatalog("hms", "/other/hms");
    }
    catch (const JException&) {
        duplicate_rejected = true;
    }
    assert(duplicate_rejected);

    bool empty_name_rejected = false;
    try {
        catalogs.addCatalog("", "/config/empty");
    }
    catch (const JException&) {
        empty_name_rejected = true;
    }
    assert(empty_name_rejected);

    const auto providers = catalogs.freeze();
    assert(providers.size() == 1);
    assert(providers[0].name == "hms");
    assert(providers[0].directory == "/config/hms");

    bool frozen_registry_rejected = false;
    try {
        catalogs.addCatalog("late", "/config/late");
    }
    catch (const JException&) {
        frozen_registry_rejected = true;
    }
    assert(frozen_registry_rejected);
}
