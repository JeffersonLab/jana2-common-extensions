#pragma once

#include <JANA/JEventProcessor.h>

#include "FADC250PulseHit.h"
#include "JEventService_DetectorTranslatorsMap.h"
#include "JEventService_TranslationTable.h"

class JEventProcessor_DetectorDigiHits : public JEventProcessor {
public:
    JEventProcessor_DetectorDigiHits();

    void ProcessParallel(const JEvent& event) override;

private:
    Input<FADC250PulseHit> m_fadcPulses {this};
    Service<JEventService_DetectorTranslatorsMap> m_detectorTranslators {this};
    Service<JEventService_TranslationTable> m_translationTables {this};
};
