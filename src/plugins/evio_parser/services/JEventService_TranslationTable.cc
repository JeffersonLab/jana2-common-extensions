#include "JEventService_TranslationTable.h"

#include <algorithm>
#include <filesystem>
#include <iterator>
#include <limits>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "translation_table/DetectorMappingManifest.h"

namespace {

struct DetectorManifest {
    std::filesystem::path directory;
    std::vector<DetectorMappingRunRange> ranges;
};

const DetectorMappingRunRange* findRange(
    const std::vector<DetectorMappingRunRange>& ranges,
    std::uint64_t run_number) {
    const auto upper = std::upper_bound(
        ranges.begin(), ranges.end(), run_number,
        [](std::uint64_t run, const DetectorMappingRunRange& range) {
            return run < range.run_min;
        });
    if (upper == ranges.begin()) {
        return nullptr;
    }

    const auto& range = *std::prev(upper);
    return run_number <= range.run_max ? &range : nullptr;
}

} // namespace

JEventService_TranslationTable::JEventService_TranslationTable(
    std::string default_mapping_directory)
    : m_mapping_directory(
        this,
        "TRANSLATION:DIRECTORY",
        std::move(default_mapping_directory),
        "Directory containing the detector mapping catalog and manifests",
        true) {
}

void JEventService_TranslationTable::Init() {
    namespace fs = std::filesystem;

    const fs::path mapping_directory(m_mapping_directory());
    const auto catalog = loadDetectorMappingCatalog(
        (mapping_directory / "manifest.map").string());

    std::vector<DetectorManifest> detector_manifests;
    std::set<std::uint64_t> boundaries;
    for (const auto& entry : catalog) {
        const auto manifest_path = mapping_directory / entry.manifest_file;
        auto ranges = loadDetectorMappingManifest(manifest_path.string());
        for (const auto& range : ranges) {
            boundaries.insert(range.run_min);
            if (range.run_max != std::numeric_limits<std::uint64_t>::max()) {
                boundaries.insert(range.run_max + 1);
            }
        }
        detector_manifests.push_back({manifest_path.parent_path(), std::move(ranges)});
    }

    std::map<std::vector<std::string>, std::shared_ptr<const TranslationTable>> tables;
    std::vector<RunRangeTable> run_tables;
    for (auto boundary = boundaries.begin(); boundary != boundaries.end(); ++boundary) {
        const auto run_min = *boundary;
        const auto next = std::next(boundary);
        const auto run_max = next == boundaries.end()
            ? std::numeric_limits<std::uint64_t>::max()
            : *next - 1;

        std::vector<std::string> mapping_files;
        for (const auto& manifest : detector_manifests) {
            const auto* range = findRange(manifest.ranges, run_min);
            if (range != nullptr) {
                mapping_files.push_back(
                    (manifest.directory / range->mapping_file).string());
            }
        }
        if (mapping_files.empty()) {
            continue;
        }

        auto [table_entry, inserted] = tables.emplace(mapping_files, nullptr);
        if (inserted) {
            auto table = std::make_shared<TranslationTable>();
            for (const auto& mapping_file : mapping_files) {
                table->LoadMappingFile(mapping_file);
            }
            table_entry->second = std::move(table);
        }

        if (!run_tables.empty() &&
            run_tables.back().run_max != std::numeric_limits<std::uint64_t>::max() &&
            run_tables.back().run_max + 1 == run_min &&
            run_tables.back().table == table_entry->second) {
            run_tables.back().run_max = run_max;
        } else {
            run_tables.push_back({run_min, run_max, table_entry->second});
        }
    }
    if (run_tables.empty()) {
        throw std::runtime_error(
            "Detector mapping catalog produces no run tables: " + m_mapping_directory());
    }
    m_run_tables = std::move(run_tables);
    m_cached_run_table.store(nullptr, std::memory_order_relaxed);
}

const TranslationTable&
JEventService_TranslationTable::getTable(std::uint64_t run_number) const {
    if (m_run_tables.empty()) {
        throw std::logic_error(
            "JEventService_TranslationTable has not been initialized");
    }

    const auto* cached = m_cached_run_table.load(std::memory_order_relaxed);
    if (cached != nullptr &&
        cached->run_min <= run_number && run_number <= cached->run_max) {
        return *cached->table;
    }

    const auto upper = std::upper_bound(
        m_run_tables.begin(), m_run_tables.end(), run_number,
        [](std::uint64_t run, const RunRangeTable& range) {
            return run < range.run_min;
        });
    if (upper == m_run_tables.begin()) {
        throw std::runtime_error(
            "No detector translation table for run " + std::to_string(run_number));
    }

    const auto& range = *std::prev(upper);
    if (run_number > range.run_max) {
        throw std::runtime_error(
            "No detector translation table for run " + std::to_string(run_number));
    }
    m_cached_run_table.store(&range, std::memory_order_relaxed);
    return *range.table;
}
