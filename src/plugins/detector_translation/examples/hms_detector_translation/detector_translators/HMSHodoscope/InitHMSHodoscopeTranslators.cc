#include "InitHMSHodoscopeTranslators.h"
#include "HMSHodoscopeIdentity.h"

#include "FADC250HallBPulseIntegralHit.h"
#include "FADC250HallBPulsePeakHit.h"
#include "FADC250HallBPulseTimeHit.h"
#include "FADC250PulseHit.h"
#include "FADC250WaveformHit.h"
#include "FADCScalerHit.h"
#include "FADCScalerTranslator.h"
#include "FADCTranslator.h"
#include "FADCDumpWriter.h"
#include "FADCScalerDumpWriter.h"
#include "JEventService_DetectorTranslatorsMap.h"

void InitHMSHodoscopeTranslators(
    JEventService_DetectorTranslatorsMap& translators) {
    translators.addTranslator<FADC250PulseHit, HMSHodoscopeFADCPulseDigiHit>(
        HMS_HODOSCOPE_DETECTOR_NAME,
        makeHMSHodoscopeFADCPulseDigiHit,
        HMSHodoscopeFADCPulseCSVHeader,
        writeHMSHodoscopeFADCPulseCSVRow);
    translators.addTranslator<FADC250WaveformHit, HMSHodoscopeFADCWaveformDigiHit>(
        HMS_HODOSCOPE_DETECTOR_NAME,
        makeHMSHodoscopeFADCWaveformDigiHit,
        HMSHodoscopeFADCWaveformCSVHeader,
        writeHMSHodoscopeFADCWaveformCSVRow);
    translators.addTranslator<FADC250HallBPulseIntegralHit, HMSHodoscopeFADCPulseIntegralDigiHit>(
        HMS_HODOSCOPE_DETECTOR_NAME,
        makeHMSHodoscopeFADCPulseIntegralDigiHit,
        HMSHodoscopeFADCPulseIntegralCSVHeader,
        writeHMSHodoscopeFADCPulseIntegralCSVRow);
    translators.addTranslator<FADC250HallBPulseTimeHit, HMSHodoscopeFADCPulseTimeDigiHit>(
        HMS_HODOSCOPE_DETECTOR_NAME,
        makeHMSHodoscopeFADCPulseTimeDigiHit,
        HMSHodoscopeFADCPulseTimeCSVHeader,
        writeHMSHodoscopeFADCPulseTimeCSVRow);
    translators.addTranslator<FADC250HallBPulsePeakHit, HMSHodoscopeFADCPulsePeakDigiHit>(
        HMS_HODOSCOPE_DETECTOR_NAME,
        makeHMSHodoscopeFADCPulsePeakDigiHit,
        HMSHodoscopeFADCPulsePeakCSVHeader,
        writeHMSHodoscopeFADCPulsePeakCSVRow);
    translators.addTranslator<FADCScalerHit, HMSHodoscopeFADCScalerDigiHit>(
        HMS_HODOSCOPE_DETECTOR_NAME,
        makeHMSHodoscopeFADCScalerDigiHit,
        HMSHodoscopeFADCScalerCSVHeader,
        writeHMSHodoscopeFADCScalerCSVRow);
}
