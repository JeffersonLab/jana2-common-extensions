#pragma once

#include <JANA/JException.h>
#include <JANA/JService.h>

#include <fstream>
#include <map>
#include <mutex>
#include <sstream>
#include <string>

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
    mutable std::mutex m_mutex;
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
        bool loaded_route = false;
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;
            std::istringstream iss(line);
            int module_id, bank_id;
            if (iss >> module_id >> bank_id) {
                addRoute(bank_id, module_id);
                loaded_route = true;
            }
        }
        if (!loaded_route) {
            throw JException("JEventService_BankToModuleMap: No data in mapping file: %s", filename.c_str());
        }
    }

    /// Register a bank ID to module ID route.
    void addRoute(int bank_id, int module_id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_frozen) {
            throw JException("Bank-to-module route registry is already frozen");
        }
        if (!m_bank_to_module.emplace(bank_id, module_id).second) {
            throw JException("Duplicate bank-to-module route for bank ID %d", bank_id);
        }
    }

    /// Look up the module ID for a bank ID. Returns -1 if not found.
    int getModuleId(int bank_id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_frozen = true;
        auto it = m_bank_to_module.find(bank_id);
        if (it == m_bank_to_module.end()) return -1;
        return it->second;
    }
};
