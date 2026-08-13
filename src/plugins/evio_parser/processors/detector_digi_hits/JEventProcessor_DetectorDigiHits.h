#pragma once

#include <JANA/JEventProcessor.h>

#include "FADC250HallBPulseIntegralHit.h"
#include "FADC250HallBPulsePeakHit.h"
#include "FADC250HallBPulseTimeHit.h"
#include "FADC250PulseHit.h"
#include "FADC250WaveformHit.h"
#include "FADCScalerHit.h"
#include "JEventService_DetectorTranslatorsMap.h"
#include "JEventService_TranslationTable.h"

class JEventProcessor_DetectorDigiHits : public JEventProcessor {
public:
    JEventProcessor_DetectorDigiHits();

    void ProcessParallel(const JEvent& event) override;

private:
    Input<FADC250PulseHit> m_fadcPulses {this};
    Input<FADC250WaveformHit> m_fadcWaveforms {this};
    Input<FADC250HallBPulseIntegralHit> m_fadcPulseIntegrals {this};
    Input<FADC250HallBPulseTimeHit> m_fadcPulseTimes {this};
    Input<FADC250HallBPulsePeakHit> m_fadcPulsePeaks {this};
    Input<FADCScalerHit> m_fadcScalers {this};
    Service<JEventService_DetectorTranslatorsMap> m_detectorTranslators {this};
    Service<JEventService_TranslationTable> m_translationTables {this};
};
