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
        "",
        "Core mapping file override (directory additions still apply), lines: 'module_id bank_id'", true};
    Parameter<std::string> m_core_config_dir {this, "JCE:CORE_CONFIG_DIR",
        JCE_INSTALL_CONFIG_DIR, "Core configuration directory", true};
    std::map<int, std::string> m_route_sources;
    mutable std::mutex m_mutex;
    bool m_frozen = false;

public:
    void Init() override {
        for (const auto& file : jce_config_files("mapping.db", m_core_config_dir(),
                                               m_bank_to_module_file(), m_logger)) {
            loadMappingFile(file);
        }
    }

    void loadMappingFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) {
            throw JException("Failed to open mapping file: %s", filename.c_str());
        }
        // Validate a whole layer before publishing any of its routes.
        std::map<int, std::pair<int, std::string>> routes;
        std::string line;
        unsigned line_number = 0;
        while (std::getline(file, line)) {
            ++line_number;
            line = line.substr(0, line.find('#'));
            std::istringstream row(line);
            row >> std::ws;
            if (row.eof()) continue;
            int module_id, bank_id;
            std::string extra;
            const auto source = filename + ":" + std::to_string(line_number);
            if (!(row >> module_id >> bank_id) || (row >> extra) ||
                module_id < 0 || bank_id < 0 || bank_id > 65535) {
                throw JException("Malformed mapping row at %s", source.c_str());
            }
            const auto [it, inserted] = routes.emplace(bank_id, std::make_pair(module_id, source));
            if (!inserted && it->second.first != module_id) {
                throw JException("Conflicting mapping for bank %d at %s and %s",
                                 bank_id, it->second.second.c_str(), source.c_str());
            }
        }
        if (file.bad()) throw JException("Failed reading mapping file: %s", filename.c_str());
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_frozen) throw JException("Bank-to-module route registry is already frozen");
        for (const auto& [bank, route] : routes) {
            const auto existing = m_bank_to_module.find(bank);
            if (existing != m_bank_to_module.end() && existing->second == route.first) continue;
            if (existing != m_bank_to_module.end()) {
                const auto previous = m_route_sources.find(bank);
                jce_config_warning(m_logger,
                    "Bank " + std::to_string(bank) + " mapping overridden: module " +
                    std::to_string(existing->second) + " (" +
                    (previous == m_route_sources.end() ? "plugin registration" : previous->second) +
                    ") -> " + std::to_string(route.first) + " (" + route.second + ")");
            }
            m_bank_to_module[bank] = route.first;
            m_route_sources[bank] = route.second;
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
