#include <JANA/JApplication.h>

#include "FADC250PulseHit.h"
#include "FADCScalerHit.h"
#include "FADCScalerTranslator.h"
#include "FADCTranslator.h"
#include "JEventService_DetectorTranslatorsMap.h"

void InitDetectorTranslators(JApplication* app) {
    auto translators =
        app->GetService<JEventService_DetectorTranslatorsMap>();

    translators->addTranslator<FADC250PulseHit>(
        "HMS_HODOSCOPE",
        translateHMSHodoscopeFADCPulseHit);
    translators->addTranslator<FADC250WaveformHit>(
        "HMS_HODOSCOPE",
        translateHMSHodoscopeFADCWaveformHit);
    translators->addTranslator<FADC250HallBPulseIntegralHit>(
        "HMS_HODOSCOPE",
        translateHMSHodoscopeFADCPulseIntegralHit);
    translators->addTranslator<FADC250HallBPulseTimeHit>(
        "HMS_HODOSCOPE",
        translateHMSHodoscopeFADCPulseTimeHit);
    translators->addTranslator<FADC250HallBPulsePeakHit>(
        "HMS_HODOSCOPE",
        translateHMSHodoscopeFADCPulsePeakHit);
    translators->addTranslator<FADCScalerHit>(
        "HMS_HODOSCOPE",
        translateHMSHodoscopeFADCScalerHit);

    translators->freeze();
}
