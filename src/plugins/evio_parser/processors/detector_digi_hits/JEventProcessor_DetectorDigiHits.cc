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
    m_fadcPulses.SetOptional(true);
    m_fadcWaveforms.SetOptional(true);
    m_fadcPulseIntegrals.SetOptional(true);
    m_fadcPulseTimes.SetOptional(true);
    m_fadcPulsePeaks.SetOptional(true);
    m_fadcScalers.SetOptional(true);
}

void JEventProcessor_DetectorDigiHits::ProcessParallel(const JEvent& event) {
    const auto table = m_translationTables->GetTable(event.GetRunNumber());
    routeHits<FADC250PulseHit>(
        m_fadcPulses(), *table, *m_detectorTranslators, event);
    routeHits<FADC250WaveformHit>(
        m_fadcWaveforms(), *table, *m_detectorTranslators, event);
    routeHits<FADC250HallBPulseIntegralHit>(
        m_fadcPulseIntegrals(), *table, *m_detectorTranslators, event);
    routeHits<FADC250HallBPulseTimeHit>(
        m_fadcPulseTimes(), *table, *m_detectorTranslators, event);
    routeHits<FADC250HallBPulsePeakHit>(
        m_fadcPulsePeaks(), *table, *m_detectorTranslators, event);
    routeHits<FADCScalerHit>(
        m_fadcScalers(), *table, *m_detectorTranslators, event);
}
