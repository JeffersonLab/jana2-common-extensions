#pragma once

#include <compare>
#include <cstdint>
#include <string>
#include <vector>

struct DetectorMappingRunRange {
    std::uint64_t run_min;
    std::uint64_t run_max;
    std::string mapping_file;

    auto operator<=>(const DetectorMappingRunRange&) const = default;
};

struct DetectorMappingCatalogEntry {
    std::string detector;
    std::string manifest_file;

    auto operator<=>(const DetectorMappingCatalogEntry&) const = default;
};

std::vector<DetectorMappingRunRange> loadDetectorMappingManifest(
    const std::string& path);

std::vector<DetectorMappingCatalogEntry> loadDetectorMappingCatalog(
    const std::string& path);
