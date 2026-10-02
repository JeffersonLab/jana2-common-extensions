#pragma once

#include <JANA/JService.h>

#include <atomic>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "TranslationTable.h"
#include "JEventService_DetectorMappingCatalogs.h"

class JEventService_TranslationTable : public JService {
public:
    explicit JEventService_TranslationTable(
        std::string default_mapping_directory = "");
    explicit JEventService_TranslationTable(
        std::vector<DetectorMappingCatalogProvider> default_catalogs);

    void Init() override;

    const TranslationTable& getTable(std::uint64_t run_number) const;

private:
    struct RunRangeTable {
        std::uint64_t run_min;
        std::uint64_t run_max;
        std::shared_ptr<const TranslationTable> table;
    };
    static_assert(std::atomic<const RunRangeTable*>::is_always_lock_free);

    Parameter<std::string> m_mapping_directory;
    Service<JEventService_DetectorMappingCatalogs> m_catalogs {this};
    std::vector<DetectorMappingCatalogProvider> m_default_catalogs;
    std::vector<RunRangeTable> m_run_tables;
    mutable std::atomic<const RunRangeTable*> m_cached_run_table {nullptr};
};
