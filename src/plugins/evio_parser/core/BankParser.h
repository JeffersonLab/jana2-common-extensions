#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include <JANA/JLogger.h>
#include <JANA/JException.h>

#include "EventHits.h"
#include "PhysicsEvent.h"
#include "TriggerData.h"
#include "eviocc.h"

struct BankContext {
    std::uint32_t rocid;
    std::uint16_t description;
    std::uint8_t status;
    std::uint8_t number;
    std::uint8_t data_type;
    JLogger& logger;
};

class BankParser {
public:
    virtual ~BankParser() = default;

    // Legacy electronics-module entry point. Existing parsers may continue to
    // override this while new bank parsers use the context-rich overload.
    virtual void parse(
        std::shared_ptr<evio::BaseStructure> data_block,
        std::uint32_t rocid,
        std::vector<PhysicsEvent*>& physics_events,
        TriggerData& trigger_data) {
        throw JException("Bank parser does not implement parse()");
    }

    virtual void parse(
        std::shared_ptr<evio::BaseStructure> data_block,
        const BankContext& context,
        std::vector<PhysicsEvent*>& physics_events,
        TriggerData& trigger_data) {
        parse(std::move(data_block), context.rocid, physics_events, trigger_data);
    }

    void SetLogger(JLogger& logger) { m_logger = &logger; }

protected:
    static std::uint32_t getBitsInRange(
        std::uint32_t value,
        int high,
        int low) {
        return (value >> low) & ((1u << (high - low + 1)) - 1);
    }

    JLogger& GetLogger() const { return *m_logger; }

private:
    JLogger* m_logger = nullptr;
};
