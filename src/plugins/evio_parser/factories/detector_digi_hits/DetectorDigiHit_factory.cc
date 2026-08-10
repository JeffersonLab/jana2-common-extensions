#include "DetectorDigiHit_factory.h"

#include <JANA/JEvent.h>

#include "HMSHodoscopeTranslator.h"

DetectorDigiHit_factory::DetectorDigiHit_factory() {
    SetTypeName("DetectorDigiHit_factory");
    SetPrefix("detector_digi_hits");
}

void DetectorDigiHit_factory::ChangeRun(const JEvent& event) {
    m_translation_table =
        m_translation_table_service->GetTable(event.GetRunNumber());
}

void DetectorDigiHit_factory::Process(const JEvent&) {
    const auto& fadc_pulses = m_fadc_pulses_in();
    auto& hms_hodoscope_hits = m_hms_hodoscope_hits_out();
    hms_hodoscope_hits.reserve(fadc_pulses.size());

    for (const auto* pulse : fadc_pulses) {
        const auto* address = m_translation_table->Lookup({
            pulse->rocid,
            pulse->slot,
            pulse->chan
        });
        if (address == nullptr || address->detector != "HMS_HODOSCOPE") {
            continue;
        }

        hms_hodoscope_hits.push_back(
            new HMSHodoscopeDigiHit(MakeHMSHodoscopeDigiHit(*pulse, *address)));
    }
}
