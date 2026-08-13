#pragma once

#include <JANA/JEventProcessor.h>

#include <fstream>
#include <string>

#include "FADCPulseDigiHit.h"

class JEventProcessor_DetectorTranslationDump : public JEventProcessor {
public:
    JEventProcessor_DetectorTranslationDump();

    void Init() override;
    void ProcessSequential(const JEvent& event) override;
    void Finish() override;

private:
    Input<HMSHodoscopeFADCPulseDigiHit> m_hits {this};

    Parameter<std::string> m_outputFilename {
        this,
        "OUTPUT",
        "detector_translation_dump.csv",
        "CSV output file for translated HMS hodoscope DigiHits"
    };

    std::ofstream m_output;
};
