#pragma once

#include <JANA/JEventProcessor.h>
#include "JEventService_DetectorTranslatorsMap.h"

#include <fstream>
#include <string>
#include <vector>

class JEventProcessor_DetectorTranslationDump : public JEventProcessor {
public:
    JEventProcessor_DetectorTranslationDump();
    void ProcessSequential(const JEvent& event) override;
    void Finish() override;

private:
    void openOutputs();
    bool m_outputs_opened = false;
    Service<JEventService_DetectorTranslatorsMap> m_registry {this};
    Parameter<std::string> m_outputDirectory {
        this, "OUTPUT_DIRECTORY", "detector_translation_dump",
        "Directory containing one CSV file per detector/raw-hit translation route"
    };
    std::vector<DetectorTranslationDump::Output> m_writers;
    std::vector<std::ofstream> m_outputs;
};
