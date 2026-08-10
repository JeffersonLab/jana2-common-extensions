#pragma once

#include <JANA/JEventProcessor.h>

#include <fstream>
#include <string>

#include "HMSHodoscopeDigiHit.h"

class JEventProcessor_DetectorTranslationTest : public JEventProcessor {
public:
    JEventProcessor_DetectorTranslationTest();

    void Init() override;
    void ProcessSequential(const JEvent& event) override;
    void Finish() override;

private:
    Parameter<std::string> m_outputFilename {
        this,
        "OUTPUT",
        "detector_translation_test.csv",
        "CSV output file for translated HMS hodoscope DigiHits"
    };

    std::ofstream m_output;
};
