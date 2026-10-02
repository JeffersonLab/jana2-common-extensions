#pragma once

#include <JANA/JException.h>
#include <JANA/JService.h>

#include <mutex>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

struct DetectorMappingCatalogProvider {
    std::string name;
    std::string directory;
};

class JEventService_DetectorMappingCatalogs : public JService {
public:
    void addCatalog(std::string name, std::string directory) {
        std::scoped_lock lock(m_mutex);
        if (m_frozen) {
            throw JException("Detector mapping catalog registry is already frozen");
        }
        if (name.empty()) {
            throw JException("Detector mapping catalog provider name is empty");
        }
        if (directory.empty()) {
            throw JException(
                "Detector mapping catalog directory for provider '%s' is empty",
                name.c_str());
        }
        if (!m_names.insert(name).second) {
            throw JException(
                "Duplicate detector mapping catalog provider '%s'",
                name.c_str());
        }
        m_catalogs.push_back({std::move(name), std::move(directory)});
    }

    std::vector<DetectorMappingCatalogProvider> freeze() {
        std::scoped_lock lock(m_mutex);
        m_frozen = true;
        return m_catalogs;
    }

private:
    std::vector<DetectorMappingCatalogProvider> m_catalogs;
    std::unordered_set<std::string> m_names;
    std::mutex m_mutex;
    bool m_frozen = false;
};
