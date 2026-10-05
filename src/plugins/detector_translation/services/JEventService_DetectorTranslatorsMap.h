#pragma once

#include <JANA/JException.h>
#include <JANA/JEvent.h>
#include <JANA/JService.h>

#include <DAQAddressable.h>
#include <DetectorAddress.h>
#include <TranslationTable.h>
#include "DetectorTranslationDump.h"

#include <atomic>
#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

class JEventService_DetectorTranslatorsMap : public JService {
public:
    template <typename RawHitT, typename DigiHitT>
    using Translator = std::function<DigiHitT(const RawHitT&, const DetectorAddress&)>;

    template <typename RawHitT, typename DigiHitT>
    void addTranslator(
        std::string detector,
        Translator<RawHitT, DigiHitT> translator,
        std::string csvHeader,
        DetectorTranslationDump::RowWriter<DigiHitT> writeRow) {
        static_assert(DAQAddressable<RawHitT>,
            "Raw hit types must provide getDAQAddress(hit)");
        std::scoped_lock lock(m_mutex);
        if (m_frozen.load()) {
            throw JException("Detector translators map is already frozen");
        }
        if (detector.empty() || !translator || !writeRow) {
            throw JException("A detector route requires a detector, translator, and writer");
        }
        auto& translators = getOrCreateTranslators<RawHitT>();
        if (translators.contains(detector)) {
            throw JException("Duplicate detector route for raw hit type '%s' and detector '%s'",
                typeid(RawHitT).name(), detector.c_str());
        }
        auto route = std::make_shared<Route<RawHitT, DigiHitT>>(std::move(translator));
        m_dump.addWriter<RawHitT, DigiHitT>(detector, std::move(csvHeader), std::move(writeRow));
        translators.emplace(std::move(detector), std::move(route));
    }

    void freeze() {
        std::scoped_lock lock(m_mutex);
        m_frozen.store(true);
    }

    std::vector<DetectorTranslationDump::Output> getDumpOutputs() {
        std::scoped_lock lock(m_mutex);
        m_frozen.store(true);
        return m_dump.outputs();
    }

    void translateEvent(const TranslationTable& table, const JEvent& event) const {
        {
            std::scoped_lock lock(m_mutex);
            m_frozen.store(true);
        }
        for (const auto& entry : m_translators) {
            entry.second->translate(table, event);
        }
    }

    template <typename RawHitT>
    std::size_t getTranslatorCount() const {
        const auto entry = m_translators.find(std::type_index(typeid(RawHitT)));
        return entry == m_translators.end() ? 0 :
            static_cast<const TranslatorMap<RawHitT>*>(entry->second.get())->translators.size();
    }

private:
    template <typename RawHitT>
    using MatchedHits = std::vector<std::pair<const RawHitT*, const DetectorAddress*>>;

    template <typename RawHitT>
    struct RouteInterface {
        virtual ~RouteInterface() = default;
        virtual void translate(const MatchedHits<RawHitT>&,
            const JEvent&, const std::string& detector) const = 0;
    };

    template <typename RawHitT, typename DigiHitT>
    struct Route final : RouteInterface<RawHitT> {
        Translator<RawHitT, DigiHitT> translator;
        explicit Route(Translator<RawHitT, DigiHitT> convert)
            : translator(std::move(convert)) {}

        void translate(const MatchedHits<RawHitT>& hits,
            const JEvent& event, const std::string& detector) const override {
            auto output = std::make_unique<DetectorTranslationOutput<RawHitT, DigiHitT>>();
            output->hits.reserve(hits.size());
            for (const auto& [raw, address] : hits) {
                auto hit = std::make_unique<DigiHitT>(translator(*raw, *address));
                const auto* pointer = hit.get();
                event.Insert(hit.release());
                output->hits.push_back(pointer);
            }
            event.Insert(output.release(), detector);
        }
    };

    struct TranslatorMapBase {
        virtual ~TranslatorMapBase() = default;
        virtual void translate(const TranslationTable&, const JEvent&) const = 0;
    };

    template <typename RawHitT>
    struct TranslatorMap final : TranslatorMapBase {
        std::unordered_map<std::string, std::shared_ptr<RouteInterface<RawHitT>>> translators;

        void translate(const TranslationTable& table, const JEvent& event) const override {
            // Group matched raw hits with event-local state. Each raw hit gets
            // one table lookup, and shared routes remain immutable.
            std::unordered_map<std::string, MatchedHits<RawHitT>> matched;
            for (const auto* hit : event.Get<RawHitT>("", false)) {
                const auto* address = table.Lookup(getDAQAddress(*hit));
                if (address == nullptr || !translators.contains(address->detector)) continue;
                matched[address->detector].emplace_back(hit, address);
            }
            for (const auto& [detector, hits] : matched) {
                translators.at(detector)->translate(hits, event, detector);
            }
        }
    };

    template <typename RawHitT>
    std::unordered_map<std::string, std::shared_ptr<RouteInterface<RawHitT>>>&
    getOrCreateTranslators() {
        const auto type = std::type_index(typeid(RawHitT));
        auto entry = m_translators.find(type);
        if (entry == m_translators.end()) {
            entry = m_translators.emplace(type, std::make_unique<TranslatorMap<RawHitT>>()).first;
        }
        return static_cast<TranslatorMap<RawHitT>*>(entry->second.get())->translators;
    }

    std::unordered_map<std::type_index, std::unique_ptr<TranslatorMapBase>> m_translators;
    DetectorTranslationDump m_dump;
    mutable std::atomic_bool m_frozen = false;
    mutable std::mutex m_mutex;
};
