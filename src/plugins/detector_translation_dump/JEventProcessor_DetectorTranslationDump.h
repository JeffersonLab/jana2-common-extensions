#pragma once

#include <JANA/JEventProcessor.h>

#include <fstream>
#include <string>

#include "FADCPulseDigiHit.h"
#include "FADCPulseIntegralDigiHit.h"
#include "FADCPulsePeakDigiHit.h"
#include "FADCPulseTimeDigiHit.h"
#include "FADCScalerDigiHit.h"
#include "FADCWaveformDigiHit.h"

class JEventProcessor_DetectorTranslationDump : public JEventProcessor {
public:
    JEventProcessor_DetectorTranslationDump();

    void Init() override;
    void ProcessSequential(const JEvent& event) override;
    void Finish() override;

private:
    Input<HMSHodoscopeFADCPulseDigiHit> m_fadcPulses {this};
    Input<HMSHodoscopeFADCWaveformDigiHit> m_fadcWaveforms {this};
    Input<HMSHodoscopeFADCPulseIntegralDigiHit> m_fadcPulseIntegrals {this};
    Input<HMSHodoscopeFADCPulseTimeDigiHit> m_fadcPulseTimes {this};
    Input<HMSHodoscopeFADCPulsePeakDigiHit> m_fadcPulsePeaks {this};
    Input<HMSHodoscopeFADCScalerDigiHit> m_fadcScalers {this};

    Parameter<std::string> m_outputDirectory {
        this,
        "OUTPUT_DIRECTORY",
        "detector_translation_dump",
        "Directory containing one CSV file per translated DigiHit type"
    };

    std::ofstream m_fadcPulseOutput;
    std::ofstream m_fadcWaveformOutput;
    std::ofstream m_fadcPulseIntegralOutput;
    std::ofstream m_fadcPulseTimeOutput;
    std::ofstream m_fadcPulsePeakOutput;
    std::ofstream m_fadcScalerOutput;
};
