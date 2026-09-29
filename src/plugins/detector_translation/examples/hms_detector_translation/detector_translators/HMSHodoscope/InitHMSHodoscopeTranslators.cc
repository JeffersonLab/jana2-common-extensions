#include "InitHMSHodoscopeTranslators.h"

#include "FADC250HallBPulseIntegralHit.h"
#include "FADC250HallBPulsePeakHit.h"
#include "FADC250HallBPulseTimeHit.h"
#include "FADC250PulseHit.h"
#include "FADC250WaveformHit.h"
#include "FADCScalerHit.h"
#include "FADCScalerTranslator.h"
#include "FADCTranslator.h"
#include "JEventService_DetectorTranslatorsMap.h"

void InitHMSHodoscopeTranslators(
    JEventService_DetectorTranslatorsMap& translators) {
    translators.addTranslator<FADC250PulseHit>(
        "HMS_HODOSCOPE",
        translateHMSHodoscopeFADCPulseHit);
    translators.addTranslator<FADC250WaveformHit>(
        "HMS_HODOSCOPE",
        translateHMSHodoscopeFADCWaveformHit);
    translators.addTranslator<FADC250HallBPulseIntegralHit>(
        "HMS_HODOSCOPE",
        translateHMSHodoscopeFADCPulseIntegralHit);
    translators.addTranslator<FADC250HallBPulseTimeHit>(
        "HMS_HODOSCOPE",
        translateHMSHodoscopeFADCPulseTimeHit);
    translators.addTranslator<FADC250HallBPulsePeakHit>(
        "HMS_HODOSCOPE",
        translateHMSHodoscopeFADCPulsePeakHit);
    translators.addTranslator<FADCScalerHit>(
        "HMS_HODOSCOPE",
        translateHMSHodoscopeFADCScalerHit);
}
