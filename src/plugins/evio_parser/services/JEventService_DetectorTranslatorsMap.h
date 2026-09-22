#pragma once

#include <JANA/JException.h>
#include <JANA/JEvent.h>
#include <JANA/JService.h>

#include <DAQAddressable.h>
#include <DetectorAddress.h>
#include <TranslationTable.h>

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <utility>

class JEvent;

class JEventService_DetectorTranslatorsMap : public JService {
public:
    template <typename RawHitT>
    using Translator = std::function<void(
        const RawHitT&,
        const DetectorAddress&,
        const JEvent&)>;

    template <typename RawHitT>
    void addTranslator(
        std::string detector,
        Translator<RawHitT> translator) {
        static_assert(DAQAddressable<RawHitT>,
            "Raw hit types must provide getDAQAddress(hit)");

        std::scoped_lock lock(m_mutex);
        if (m_frozen.load()) {
            throw JException("Detector translators map is already frozen");
        }

        auto& translators = getOrCreateTranslators<RawHitT>();
        const auto detectorName = detector;
        if (!translators.emplace(
                std::move(detector),
                std::move(translator)).second) {
            throw JException(
                "Duplicate detector route for raw hit type '%s' and detector '%s'",
                typeid(RawHitT).name(),
                detectorName.c_str());
        }
    }

    void freeze() {
        std::scoped_lock lock(m_mutex);
        m_frozen.store(true);
    }

    void translateEvent(
        const TranslationTable& table,
        const JEvent& event) const {
        {
            std::scoped_lock lock(m_mutex);
            m_frozen.store(true);
        }
        for (const auto& entry : m_translators) {
            entry.second->translate(table, event);
        }
    }

    template <typename RawHitT>
    const std::unordered_map<std::string, Translator<RawHitT>>&
    getTranslators() const {
        const auto translators = m_translators.find(
            std::type_index(typeid(RawHitT)));
        if (translators == m_translators.end()) {
            static const std::unordered_map<std::string, Translator<RawHitT>>
                empty;
            return empty;
        }
        return static_cast<const TranslatorMap<RawHitT>*>(
            translators->second.get())->translators;
    }

private:
    struct TranslatorMapBase {
        virtual ~TranslatorMapBase() = default;
        virtual void translate(
            const TranslationTable& table,
            const JEvent& event) const = 0;
    };

    template <typename RawHitT>
    struct TranslatorMap final : TranslatorMapBase {
        std::unordered_map<std::string, Translator<RawHitT>> translators;

        void translate(
            const TranslationTable& table,
            const JEvent& event) const override {
            for (const auto* hit : event.Get<RawHitT>("", false)) {
                const auto* address = table.Lookup(getDAQAddress(*hit));
                if (address == nullptr) {
                    continue;
                }

                const auto translator = translators.find(address->detector);
                if (translator != translators.end()) {
                    translator->second(*hit, *address, event);
                }
            }
        }
    };

    template <typename RawHitT>
    std::unordered_map<std::string, Translator<RawHitT>>&
    getOrCreateTranslators() {
        const auto type = std::type_index(typeid(RawHitT));
        auto existing = m_translators.find(type);
        if (existing != m_translators.end()) {
            return static_cast<TranslatorMap<RawHitT>*>(existing->second.get())
                ->translators;
        }

        auto map = std::make_unique<TranslatorMap<RawHitT>>();
        auto& translators = map->translators;
        m_translators.emplace(type, std::move(map));
        return translators;
    }

    std::unordered_map<std::type_index, std::unique_ptr<TranslatorMapBase>>
        m_translators;
    mutable std::atomic_bool m_frozen = false;
    mutable std::mutex m_mutex;
};
