#pragma once

#include <JANA/JService.h>
#include <map>
#include <mutex>
#include <fstream>
#include <sstream>

#include "jce_config_paths.h"

/**
 * @class JEventService_BankToModuleMap
 * @brief JANA service mapping bank IDs to module IDs
 */
class JEventService_BankToModuleMap : public JService {

    std::map<int, int> m_bank_to_module;
    Parameter<std::string> m_bank_to_module_file {this, "BANKMAP:FILE",
        jce_config_path("mapping.db", "BANKMAP:FILE"),
        "Mapping file with lines: 'module_id bank_id'", true};
    std::mutex m_mutex;
    bool m_frozen = false;

public:
    void Init() override {
        if (!m_bank_to_module_file().empty()) {
            loadMappingFile(m_bank_to_module_file());
        }
    }

    void loadMappingFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) {
            throw JException("JEventService_BankToModuleMap: Failed to open mapping file: %s", filename.c_str());
        }
        // Load the mappings from the file.
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;
            std::istringstream iss(line);
            int module_id, bank_id;
            if (iss >> module_id >> bank_id) {
                addRoute(bank_id, module_id);
            }
        }
        // If nothing after the header, throw an error.
        if (m_bank_to_module.empty()) {
            throw JException("JEventService_BankToModuleMap: No data in mapping file: %s", filename.c_str());
        }
    }

    /// Add a bank-description to parser-ID route programmatically. Setup
    /// plugins use this for experiment-owned banks such as 0xace.
    void addRoute(int bank_id, int parser_id) {
        std::scoped_lock lock(m_mutex);
        if (m_frozen) {
            throw JException("Bank-to-parser map is already frozen");
        }
        if (!m_bank_to_module.emplace(bank_id, parser_id).second) {
            throw JException("Duplicate bank mapping for bank ID 0x%x", bank_id);
        }
    }

    void addMapping(int bank_id, int parser_id) {
        addRoute(bank_id, parser_id);
    }

    /// Look up parser ID for a bank ID. Returns -1 if not found.
    int getParserId(int bank_id) {
        std::scoped_lock lock(m_mutex);
        m_frozen = true;
        auto it = m_bank_to_module.find(bank_id);
        if (it == m_bank_to_module.end()) return -1;
        return it->second;
    }

    int getModuleId(int bank_id) {
        return getParserId(bank_id);
    }
};
