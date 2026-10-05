#include "JEventProcessor_DetectorTranslationDump.h"

#include <JANA/JException.h>
#include <filesystem>

JEventProcessor_DetectorTranslationDump::JEventProcessor_DetectorTranslationDump() {
    SetTypeName("JEventProcessor_DetectorTranslationDump");
    SetPrefix("detector_translation_dump");
    SetCallbackStyle(CallbackStyle::ExpertMode);
}

void JEventProcessor_DetectorTranslationDump::Init() {
    m_writers = m_registry->getDumpOutputs();
    for (const auto& writer : m_writers) {
        const auto directory = std::filesystem::path(m_outputDirectory()) / writer.detector;
        std::error_code error;
        std::filesystem::create_directories(directory, error);
        if (error) {
            throw JException("Failed to create detector dump directory '%s': %s",
                directory.string().c_str(), error.message().c_str());
        }
        const auto path = directory / writer.filename;
        std::ofstream output;
        output.exceptions(std::ios::failbit | std::ios::badbit);
        try {
            output.open(path);
            output << writer.header << '\n';
        } catch (const std::ios_base::failure& error) {
            throw JException("Failed to open detector dump output '%s': %s",
                path.string().c_str(), error.what());
        }
        m_outputs.push_back(std::move(output));
    }
}

void JEventProcessor_DetectorTranslationDump::ProcessSequential(const JEvent& event) {
    for (std::size_t index = 0; index < m_writers.size(); ++index) {
        m_writers[index].writeEvent(m_outputs[index], event);
    }
}

void JEventProcessor_DetectorTranslationDump::Finish() {
    for (auto& output : m_outputs) {
        output.close();
    }
}
