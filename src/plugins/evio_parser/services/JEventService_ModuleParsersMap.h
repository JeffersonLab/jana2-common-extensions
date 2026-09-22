#pragma once

#include <JANA/JService.h>
#include "BankParser.h"
#include <map>
#include <memory>
#include <mutex>

/**
 * @class JEventService_ModuleParsersMap
 * @brief JANA service mapping module IDs to parser implementations
 */
class JEventService_ModuleParsersMap : public JService {

    std::map<int, std::shared_ptr<BankParser>> m_module_parsers;
    mutable std::mutex m_mutex;
    bool m_frozen = false;

public:
    /// Register a parser implementation for a given module ID
    void addParser(int module_id, std::shared_ptr<BankParser> parser) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_frozen) {
            throw JException("Bank parser registry is already frozen");
        }
        if (parser == nullptr) {
            throw JException("Bank parser ID %d is null", module_id);
        }
        parser->SetLogger(GetLogger());
        if (!m_module_parsers.emplace(module_id, std::move(parser)).second) {
            throw JException("Duplicate bank parser ID %d", module_id);
        }
    }

    /// Look up a parser implementation for a given module ID
    std::shared_ptr<BankParser> getParser(int module_id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_frozen = true;
        auto it = m_module_parsers.find(module_id);
        return it != m_module_parsers.end() ? it->second : nullptr;
    }
};
