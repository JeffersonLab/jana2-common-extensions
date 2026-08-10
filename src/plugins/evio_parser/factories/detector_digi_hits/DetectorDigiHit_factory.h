#pragma once

#include <JANA/JFactory.h>

#include <memory>

#include "FADC250PulseHit.h"
#include "HMSHodoscopeDigiHit.h"
#include "TranslationTableService.h"

HMSHodoscopeDigiHit MakeHMSHodoscopeDigiHit(
    const FADC250PulseHit& pulse,
    const DetectorAddress& address);

class DetectorDigiHit_factory : public JFactory {
public:
    DetectorDigiHit_factory();

    void ChangeRun(const JEvent& event) override;
    void Process(const JEvent& event) override;

private:
    Input<FADC250PulseHit> m_fadc_pulses_in {this};
    Output<HMSHodoscopeDigiHit> m_hms_hodoscope_hits_out {this};
    Service<TranslationTableService> m_translation_table_service {this};

    std::shared_ptr<const TranslationTable> m_translation_table;
};
