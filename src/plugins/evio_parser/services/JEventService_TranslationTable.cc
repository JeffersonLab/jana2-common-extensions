#include "JEventService_TranslationTable.h"

#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <utility>
#include <vector>

JEventService_TranslationTable::JEventService_TranslationTable(
    std::string default_mapping_directory)
    : m_mapping_directory(
        this,
        "TRANSLATION:DIRECTORY",
        std::move(default_mapping_directory),
        "Directory containing detector mapping .map files",
        true) {
}

void JEventService_TranslationTable::Init() {
    namespace fs = std::filesystem;

    std::vector<fs::path> mapping_files;
    for (const auto& entry : fs::directory_iterator(m_mapping_directory())) {
        if (entry.is_regular_file() && entry.path().extension() == ".map") {
            mapping_files.push_back(entry.path());
        }
    }
    std::sort(mapping_files.begin(), mapping_files.end());
    if (mapping_files.empty()) {
        throw std::runtime_error(
            "No detector mapping .map files found in: " + m_mapping_directory());
    }

    auto table = std::make_shared<TranslationTable>();
    for (const auto& mapping_file : mapping_files) {
        table->LoadMappingFile(mapping_file.string());
    }
    m_table = std::move(table);
}

std::shared_ptr<const TranslationTable>
JEventService_TranslationTable::GetTable(std::uint64_t) const {
    if (m_table == nullptr) {
        throw std::logic_error(
            "JEventService_TranslationTable has not been initialized");
    }
    return m_table;
}
