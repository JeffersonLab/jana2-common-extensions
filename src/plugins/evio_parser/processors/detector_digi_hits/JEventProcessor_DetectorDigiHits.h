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
    Service<JEventService_DetectorTranslatorsMap> m_detectorTranslators {this};
    Service<JEventService_TranslationTable> m_translationTables {this};
};
