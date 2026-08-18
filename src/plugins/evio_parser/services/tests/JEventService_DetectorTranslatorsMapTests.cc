#include <JANA/JException.h>

#include <cassert>

#include "JEventService_DetectorTranslatorsMap.h"

namespace {

struct RawHit {};
struct OtherRawHit {};

} // namespace

int main() {
    JEventService_DetectorTranslatorsMap translators;
    translators.addTranslator<RawHit>(
        "HMS_HODOSCOPE",
        [](const auto&, const auto&, const auto&) {});
    translators.addTranslator<OtherRawHit>(
        "HMS_HODOSCOPE",
        [](const auto&, const auto&, const auto&) {});
    assert(translators.getTranslators<RawHit>().size() == 1);
    assert(translators.getTranslators<OtherRawHit>().size() == 1);

    bool duplicate_rejected = false;
    try {
        translators.addTranslator<RawHit>(
            "HMS_HODOSCOPE",
            [](const auto&, const auto&, const auto&) {});
    }
    catch (const JException&) {
        duplicate_rejected = true;
    }
    assert(duplicate_rejected);

    translators.freeze();
    bool frozen_registry_rejected = false;
    try {
        translators.addTranslator<RawHit>(
            "OTHER",
            [](const auto&, const auto&, const auto&) {});
    }
    catch (const JException&) {
        frozen_registry_rejected = true;
    }
    assert(frozen_registry_rejected);
}
