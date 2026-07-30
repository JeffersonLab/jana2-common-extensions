#include "TranslationTable.h"

#include <cjson/cJSON.h>

#include <cmath>
#include <cstdint>
#include <fstream>
#include <limits>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace {

using JsonDocument = std::unique_ptr<cJSON, decltype(&cJSON_Delete)>;

const cJSON* RequireObjectItem(const cJSON* object, const char* name, const std::string& path) {
    const auto* item = cJSON_GetObjectItemCaseSensitive(object, name);
    if (item == nullptr) {
        throw std::runtime_error("Missing '" + std::string(name) + "' in detector mapping: " + path);
    }
    return item;
}

std::string RequireString(const cJSON* object, const char* name, const std::string& path) {
    const auto* item = RequireObjectItem(object, name, path);
    if (!cJSON_IsString(item) || item->valuestring == nullptr) {
        throw std::runtime_error("'" + std::string(name) + "' must be a string in detector mapping: " + path);
    }
    return item->valuestring;
}

template <typename Integer>
Integer RequireInteger(const cJSON* object, const char* name, const std::string& path) {
    const auto* item = RequireObjectItem(object, name, path);
    if (!cJSON_IsNumber(item)
        || !std::isfinite(item->valuedouble)
        || std::trunc(item->valuedouble) != item->valuedouble
        || item->valuedouble < static_cast<double>(std::numeric_limits<Integer>::min())
        || item->valuedouble > static_cast<double>(std::numeric_limits<Integer>::max())) {
        throw std::runtime_error("'" + std::string(name) + "' must be an in-range integer in detector mapping: " + path);
    }
    return static_cast<Integer>(item->valuedouble);
}

} // namespace

bool TranslationTable::Insert(DAQAddress daq, DetectorAddress detector) {
    return m_entries.emplace(daq, detector).second;
}

const DetectorAddress* TranslationTable::Lookup(const DAQAddress& daq) const {
    const auto entry = m_entries.find(daq);
    return entry == m_entries.end() ? nullptr : &entry->second;
}

void TranslationTable::LoadJsonFile(const std::string& path) {
    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("Unable to open detector mapping: " + path);
    }

    std::ostringstream buffer;
    buffer << input.rdbuf();
    if (!input.good() && !input.eof()) {
        throw std::runtime_error("Unable to read detector mapping: " + path);
    }

    const std::string contents = buffer.str();
    JsonDocument root(
        cJSON_ParseWithLengthOpts(contents.c_str(), contents.size() + 1, nullptr, true),
        &cJSON_Delete);
    if (!root || !cJSON_IsObject(root.get())) {
        throw std::runtime_error("Invalid detector mapping JSON: " + path);
    }

    const std::string detector = RequireString(root.get(), "detector", path);
    const auto* channels = RequireObjectItem(root.get(), "channels", path);
    if (!cJSON_IsArray(channels)) {
        throw std::runtime_error("'channels' must be an array in detector mapping: " + path);
    }

    TranslationTable loaded;
    const cJSON* channel = nullptr;
    cJSON_ArrayForEach(channel, channels) {
        if (!cJSON_IsObject(channel)) {
            throw std::runtime_error("Each channel must be an object in detector mapping: " + path);
        }

        const auto* daq = RequireObjectItem(channel, "daq", path);
        const auto* detector_channel = RequireObjectItem(channel, "detector_channel", path);
        if (!cJSON_IsObject(daq) || !cJSON_IsObject(detector_channel)) {
            throw std::runtime_error(
                "'daq' and 'detector_channel' must be objects in detector mapping: " + path);
        }

        DAQAddress daq_address {
            RequireInteger<std::uint32_t>(daq, "rocid", path),
            RequireInteger<std::uint32_t>(daq, "slot", path),
            RequireInteger<std::uint32_t>(daq, "channel", path)
        };

        DetectorAddress detector_address {detector, {}};
        const cJSON* field = nullptr;
        cJSON_ArrayForEach(field, detector_channel) {
            if (field->string == nullptr) {
                throw std::runtime_error("Detector channel field has no name in detector mapping: " + path);
            }
            detector_address.fields.emplace_back(
                field->string,
                RequireInteger<std::int32_t>(detector_channel, field->string, path));
        }

        if (!loaded.Insert(std::move(daq_address), std::move(detector_address))) {
            throw std::runtime_error("Duplicate DAQ address in detector mapping: " + path);
        }
    }

    TranslationTable combined = *this;
    for (auto& [daq, address] : loaded.m_entries) {
        if (!combined.Insert(daq, std::move(address))) {
            throw std::runtime_error("Duplicate DAQ address while loading detector mapping: " + path);
        }
    }
    m_entries = std::move(combined.m_entries);
}
