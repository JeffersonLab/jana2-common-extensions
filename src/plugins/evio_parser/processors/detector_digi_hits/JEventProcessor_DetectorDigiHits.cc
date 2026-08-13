#include "JEventProcessor_DetectorDigiHits.h"

#include <JANA/JEvent.h>

#include "DAQAddressable.h"

namespace {

template <DAQAddressable HitT>
const DetectorAddress* lookupDetectorAddress(
    const TranslationTable& table,
    const HitT& hit) {
    return table.Lookup(getDAQAddress(hit));
}

} // namespace

JEventProcessor_DetectorDigiHits::JEventProcessor_DetectorDigiHits() {
    SetTypeName("JEventProcessor_DetectorDigiHits");
    SetPrefix("detector_digi_hits");
    SetCallbackStyle(CallbackStyle::ExpertMode);
}

void JEventProcessor_DetectorDigiHits::ProcessParallel(const JEvent& event) {
    const auto table = m_translationTables->GetTable(event.GetRunNumber());
    const auto& translators =
        m_detectorTranslators->getTranslators<FADC250PulseHit>();

    for (const auto* pulse : m_fadcPulses()) {
        const auto* address = lookupDetectorAddress(*table, *pulse);
        if (address == nullptr) {
            continue;
        }

        const auto translator = translators.find(address->detector);
        if (translator != translators.end()) {
            translator->second(*pulse, *address, event);
        }
    }
}
