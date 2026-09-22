#pragma once

#include <bit>
#include <cstdint>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

#include <JANA/JException.h>
#include <JANA/JService.h>
#include <JANA/Utils/JEventLevel.h>

#include "TopLevelEventDecoder.h"

struct TopLevelEventSelector {
    std::uint16_t tag_value = 0;
    std::uint16_t tag_mask = 0;
    std::optional<std::uint8_t> number;
    std::optional<std::uint8_t> data_type;

    bool matches(
        std::uint16_t tag,
        std::uint8_t event_number,
        std::uint8_t event_data_type) const {
        return (tag & tag_mask) == (tag_value & tag_mask) &&
            (!number || *number == event_number) &&
            (!data_type || *data_type == event_data_type);
    }

    int specificity() const {
        return std::popcount(tag_mask) + (number ? 8 : 0) +
            (data_type ? 8 : 0);
    }
};

struct TopLevelEventDecoderMatch {
    std::string key;
    JEventLevel level;
    std::shared_ptr<const TopLevelEventDecoder> decoder;
};

class JEventService_TopLevelEventDecoders : public JService {
public:
    void addDecoder(
        std::string key,
        TopLevelEventSelector selector,
        JEventLevel level,
        std::shared_ptr<const TopLevelEventDecoder> decoder) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_frozen) {
            throw JException("Top-level EVIO decoder registry is already frozen");
        }
        if (decoder == nullptr) {
            throw JException("Top-level EVIO decoder '%s' is null", key.c_str());
        }
        if (level != JEventLevel::Run &&
            level != JEventLevel::SlowControls) {
            throw JException(
                "Top-level EVIO decoder '%s' requested unsupported level '%s'",
                key.c_str(), toString(level).c_str());
        }
        for (const auto& entry : m_entries) {
            if (entry.key == key) {
                throw JException("Duplicate top-level EVIO decoder key '%s'", key.c_str());
            }
        }
        m_entries.push_back({
            std::move(key), selector, level, std::move(decoder)});
    }

    std::optional<TopLevelEventDecoderMatch> resolve(
        std::uint16_t tag,
        std::uint8_t number,
        std::uint8_t data_type) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_frozen = true;

        const Entry* best = nullptr;
        for (const auto& entry : m_entries) {
            if (!entry.selector.matches(tag, number, data_type)) {
                continue;
            }
            if (best != nullptr &&
                best->selector.specificity() == entry.selector.specificity()) {
                throw JException(
                    "Ambiguous top-level EVIO decoder match for tag 0x%x",
                    tag);
            }
            if (best == nullptr ||
                best->selector.specificity() < entry.selector.specificity()) {
                best = &entry;
            }
        }
        if (best == nullptr) {
            return std::nullopt;
        }
        return TopLevelEventDecoderMatch {
            best->key, best->level, best->decoder};
    }

private:
    struct Entry {
        std::string key;
        TopLevelEventSelector selector;
        JEventLevel level;
        std::shared_ptr<const TopLevelEventDecoder> decoder;
    };

    std::vector<Entry> m_entries;
    std::mutex m_mutex;
    bool m_frozen = false;
};
