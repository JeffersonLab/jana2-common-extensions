#include "JEventProcessor_DetectorDigiHits.h"

#include <JANA/JEvent.h>

#include "DAQAddressable.h"

namespace {

template <DAQAddressable HitT, typename HitRangeT>
void routeHits(
    const HitRangeT& hits,
    const TranslationTable& table,
    const JEventService_DetectorTranslatorsMap& translator_map,
    const JEvent& event) {
    const auto& translators = translator_map.getTranslators<HitT>();
    for (const auto* hit : hits) {
        const auto* address = table.Lookup(getDAQAddress(*hit));
        if (address == nullptr) {
            continue;
        }

        const auto translator = translators.find(address->detector);
        if (translator != translators.end()) {
            translator->second(*hit, *address, event);
        }
    }
}

} // namespace

JEventProcessor_DetectorDigiHits::JEventProcessor_DetectorDigiHits() {
    SetTypeName("JEventProcessor_DetectorDigiHits");
    SetPrefix("detector_digi_hits");
    SetCallbackStyle(CallbackStyle::ExpertMode);
}

void JEventProcessor_DetectorDigiHits::ProcessParallel(const JEvent& event) {
    const auto table = m_translationTables->GetTable(event.GetRunNumber());
    const auto& translator_map = m_detectorTranslators();
    routeHits<FADC250PulseHit>(
        event.Get<FADC250PulseHit>("", false), *table, translator_map, event);
    routeHits<FADC250WaveformHit>(
        event.Get<FADC250WaveformHit>("", false), *table, translator_map, event);
    routeHits<FADC250HallBPulseIntegralHit>(
        event.Get<FADC250HallBPulseIntegralHit>("", false), *table, translator_map, event);
    routeHits<FADC250HallBPulseTimeHit>(
        event.Get<FADC250HallBPulseTimeHit>("", false), *table, translator_map, event);
    routeHits<FADC250HallBPulsePeakHit>(
        event.Get<FADC250HallBPulsePeakHit>("", false), *table, translator_map, event);
    routeHits<FADCScalerHit>(
        event.Get<FADCScalerHit>("", false), *table, translator_map, event);
}
