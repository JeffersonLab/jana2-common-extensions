#pragma once

#include <JANA/JException.h>
#include <JANA/JService.h>

#include <DetectorAddress.h>

#include <functional>
#include <memory>
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
        if (m_frozen) {
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

    void freeze() { m_frozen = true; }

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
    };

    template <typename RawHitT>
    struct TranslatorMap final : TranslatorMapBase {
        std::unordered_map<std::string, Translator<RawHitT>> translators;
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
    bool m_frozen = false;
};
