#pragma once

#include <JANA/JEventProcessor.h>

#include <fstream>
#include <string>

#include "FADCDigiHit.h"

class JEventProcessor_DetectorTranslationDump : public JEventProcessor {
public:
    JEventProcessor_DetectorTranslationDump();

    void Init() override;
    void ProcessSequential(const JEvent& event) override;
    void Finish() override;

private:
    Input<HMSHodoscopeFADCDigiHit> m_hits {this};

    Parameter<std::string> m_outputFilename {
        this,
        "OUTPUT",
        "detector_translation_dump.csv",
        "CSV output file for translated HMS hodoscope DigiHits"
    };

    std::ofstream m_output;
};
