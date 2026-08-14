#pragma once

#include <JANA/JService.h>

#include <atomic>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "TranslationTable.h"
#include "jce_config_paths.h"

class JEventService_TranslationTable : public JService {
public:
    explicit JEventService_TranslationTable(
        std::string default_mapping_directory = jce_config_path(
            "evio_parser/detector_mappings",
            "TRANSLATION:DIRECTORY"));

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
    std::vector<RunRangeTable> m_run_tables;
    mutable std::atomic<const RunRangeTable*> m_cached_run_table {nullptr};
};
