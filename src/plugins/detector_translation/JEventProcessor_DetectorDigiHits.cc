#include "JEventProcessor_DetectorDigiHits.h"

JEventProcessor_DetectorDigiHits::JEventProcessor_DetectorDigiHits() {
    SetTypeName("JEventProcessor_DetectorDigiHits");
    SetPrefix("detector_digi_hits");
    SetCallbackStyle(CallbackStyle::ExpertMode);
}

void JEventProcessor_DetectorDigiHits::ProcessParallel(const JEvent& event) {
    const auto& table = m_translationTables->getTable(event.GetRunNumber());
    m_detectorTranslators->translateEvent(table, event);
}
