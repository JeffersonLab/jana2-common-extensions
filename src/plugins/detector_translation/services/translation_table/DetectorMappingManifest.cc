#include "DetectorMappingManifest.h"

#include <algorithm>
#include <charconv>
#include <fstream>
#include <limits>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

std::runtime_error manifestError(
    const std::string& path,
    std::size_t line_number,
    const std::string& message) {
    return std::runtime_error(
        "Invalid detector mapping manifest " + path + ":" +
        std::to_string(line_number) + ": " + message);
}

std::runtime_error catalogError(
    const std::string& path,
    std::size_t line_number,
    const std::string& message) {
    return std::runtime_error(
        "Invalid detector mapping catalog " + path + ":" +
        std::to_string(line_number) + ": " + message);
}

std::vector<std::string> tokenize(std::string line) {
    const auto comment = line.find('#');
    if (comment != std::string::npos) {
        line.erase(comment);
    }

    std::istringstream input(line);
    std::vector<std::string> tokens;
    for (std::string token; input >> token;) {
        tokens.push_back(std::move(token));
    }
    return tokens;
}

std::uint64_t parseRun(
    const std::string& token,
    const std::string& path,
    std::size_t line_number,
    bool allow_max) {
    if (allow_max && token == "max") {
        return std::numeric_limits<std::uint64_t>::max();
    }

    std::uint64_t run {};
    const auto [end, error] = std::from_chars(
        token.data(), token.data() + token.size(), run);
    if (error != std::errc {} || end != token.data() + token.size()) {
        throw manifestError(
            path,
            line_number,
            "run bounds must be unsigned integers; run_max may also be 'max'");
    }
    return run;
}

} // namespace

std::vector<DetectorMappingRunRange> loadDetectorMappingManifest(
    const std::string& path) {
    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("Unable to read detector mapping manifest: " + path);
    }

    std::vector<DetectorMappingRunRange> ranges;
    std::size_t line_number = 0;
    std::string line;
    while (std::getline(input, line)) {
        ++line_number;
        const auto tokens = tokenize(std::move(line));
        if (tokens.empty()) {
            continue;
        }
        if (tokens.size() != 3) {
            throw manifestError(
                path, line_number, "expected run_min run_max mapping_file");
        }

        DetectorMappingRunRange range {
            parseRun(tokens[0], path, line_number, false),
            parseRun(tokens[1], path, line_number, true),
            tokens[2]
        };
        if (range.run_min > range.run_max) {
            throw manifestError(path, line_number, "run_min exceeds run_max");
        }
        ranges.push_back(std::move(range));
    }

    if (!input.eof()) {
        throw std::runtime_error("Unable to read detector mapping manifest: " + path);
    }
    if (ranges.empty()) {
        throw std::runtime_error("Detector mapping manifest contains no run ranges: " + path);
    }

    std::sort(ranges.begin(), ranges.end(), [](const auto& left, const auto& right) {
        return left.run_min < right.run_min;
    });
    for (std::size_t index = 1; index < ranges.size(); ++index) {
        if (ranges[index].run_min <= ranges[index - 1].run_max) {
            throw std::runtime_error(
                "Overlapping run ranges in detector mapping manifest: " + path);
        }
    }
    return ranges;
}

std::vector<DetectorMappingCatalogEntry> loadDetectorMappingCatalog(
    const std::string& path) {
    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("Unable to read detector mapping catalog: " + path);
    }

    std::vector<DetectorMappingCatalogEntry> entries;
    std::set<std::string> detectors;
    std::size_t line_number = 0;
    std::string line;
    while (std::getline(input, line)) {
        ++line_number;
        const auto tokens = tokenize(std::move(line));
        if (tokens.empty()) {
            continue;
        }
        if (tokens.size() != 2) {
            throw catalogError(path, line_number, "expected detector manifest_file");
        }
        if (!detectors.insert(tokens[0]).second) {
            throw catalogError(path, line_number, "duplicate detector '" + tokens[0] + "'");
        }
        entries.push_back({tokens[0], tokens[1]});
    }

    if (!input.eof()) {
        throw std::runtime_error("Unable to read detector mapping catalog: " + path);
    }
    if (entries.empty()) {
        throw std::runtime_error("Detector mapping catalog contains no detectors: " + path);
    }

    std::sort(entries.begin(), entries.end(), [](const auto& left, const auto& right) {
        return left.detector < right.detector;
    });
    return entries;
}
