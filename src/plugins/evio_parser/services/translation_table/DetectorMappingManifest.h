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

std::vector<DetectorMappingRunRange> loadDetectorMappingManifest(
    const std::string& path);
