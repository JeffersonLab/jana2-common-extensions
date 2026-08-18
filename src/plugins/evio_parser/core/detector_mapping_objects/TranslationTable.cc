#include "TranslationTable.h"

#include <charconv>
#include <cstdint>
#include <fstream>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

std::runtime_error MappingError(
    const std::string& path,
    std::size_t line_number,
    const std::string& message) {
    return std::runtime_error(
        "Invalid detector mapping " + path + ":" + std::to_string(line_number) + ": " + message);
}

std::vector<std::string> Tokenize(std::string line) {
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

template <typename Integer>
Integer ParseInteger(
    const std::string& token,
    const std::string& path,
    std::size_t line_number) {
    Integer value {};
    const auto [end, error] = std::from_chars(
        token.data(),
        token.data() + token.size(),
        value);
    if (error != std::errc {} || end != token.data() + token.size()) {
        throw MappingError(path, line_number, "'" + token + "' is not an in-range integer");
    }
    return value;
}

std::uint32_t ParseChannel(
    const std::string& token,
    const std::string& path,
    std::size_t line_number) {
    if (token == "none") {
        return DAQAddress::UnspecifiedChannel;
    }
    return ParseInteger<std::uint32_t>(token, path, line_number);
}

} // namespace

bool TranslationTable::Insert(DAQAddress daq, DetectorAddress detector) {
    return m_entries.emplace(daq, detector).second;
}

const DetectorAddress* TranslationTable::Lookup(const DAQAddress& daq) const {
    const auto entry = m_entries.find(daq);
    return entry == m_entries.end() ? nullptr : &entry->second;
}

void TranslationTable::LoadMappingFile(const std::string& path) {
    LoadMappingFile(path, "");
}

void TranslationTable::LoadMappingFile(
    const std::string& path,
    const std::string& expected_detector) {
    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("Unable to open detector mapping: " + path);
    }

    std::string detector;
    std::vector<std::string> fields;
    TranslationTable loaded;
    std::size_t line_number = 0;
    std::string line;

    while (std::getline(input, line)) {
        ++line_number;
        const auto tokens = Tokenize(std::move(line));
        if (tokens.empty()) {
            continue;
        }

        if (tokens.front() == "detector") {
            if (tokens.size() != 2 || !detector.empty() || !fields.empty()) {
                throw MappingError(path, line_number, "expected one 'detector NAME' declaration");
            }
            if (!expected_detector.empty() && tokens[1] != expected_detector) {
                throw MappingError(
                    path,
                    line_number,
                    "detector '" + tokens[1] +
                        "' does not match catalog detector '" + expected_detector + "'");
            }
            detector = tokens[1];
            continue;
        }

        if (tokens.front() == "fields") {
            if (detector.empty() || !fields.empty() || tokens.size() < 2) {
                throw MappingError(
                    path,
                    line_number,
                    "expected one 'fields NAME [NAME ...]' declaration after detector");
            }

            std::set<std::string> unique_fields(tokens.begin() + 1, tokens.end());
            if (unique_fields.size() != tokens.size() - 1) {
                throw MappingError(path, line_number, "detector field names must be unique");
            }
            fields.assign(tokens.begin() + 1, tokens.end());
            continue;
        }

        if (detector.empty() || fields.empty()) {
            throw MappingError(path, line_number, "channel row appears before declarations");
        }
        if (tokens.size() != fields.size() + 3) {
            throw MappingError(
                path,
                line_number,
                "channel row must contain rocid, slot, channel, and one value per detector field");
        }

        DAQAddress daq {
            ParseInteger<std::uint32_t>(tokens[0], path, line_number),
            ParseInteger<std::uint32_t>(tokens[1], path, line_number),
            ParseChannel(tokens[2], path, line_number)
        };

        DetectorAddress address {detector, {}};
        address.fields.reserve(fields.size());
        for (std::size_t index = 0; index < fields.size(); ++index) {
            address.fields.emplace_back(
                fields[index],
                ParseInteger<std::int32_t>(tokens[index + 3], path, line_number));
        }

        if (!loaded.Insert(std::move(daq), std::move(address))) {
            throw MappingError(path, line_number, "duplicate DAQ address");
        }
    }

    if (!input.eof()) {
        throw std::runtime_error("Unable to read detector mapping: " + path);
    }
    if (detector.empty() || fields.empty()) {
        throw MappingError(path, line_number, "missing detector or fields declaration");
    }

    TranslationTable combined = *this;
    for (auto& [daq, address] : loaded.m_entries) {
        if (!combined.Insert(daq, std::move(address))) {
            throw std::runtime_error("Duplicate DAQ address while loading detector mapping: " + path);
        }
    }
    m_entries = std::move(combined.m_entries);
}
