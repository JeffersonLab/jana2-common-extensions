#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

struct DetectorAddress {
    std::string detector;
    std::vector<std::pair<std::string, std::int32_t>> fields;

    bool operator==(const DetectorAddress&) const = default;
};
