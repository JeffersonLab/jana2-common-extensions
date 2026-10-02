#pragma once

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include <JANA/JException.h>
#include <JANA/JLogger.h>

#include "eviocc.h"
#include "EventHits.h"
#include "PhysicsEvent.h"
#include "TriggerData.h"

/** Metadata describing the EVIO bank passed to a module parser. */
struct BankContext {
    std::uint32_t rocid;
    std::uint16_t tag;
    std::uint8_t number;
    std::uint8_t data_type;
    JLogger& logger;
};

/**
 * @class ModuleParser
 * @brief Base class for parsing module data blocks into PhysicsEvent objects
 *
 * This is the common base type for all user-defined module parsers. Each
 * concrete parser implements the @ref parse method for a specific hardware
 * module (e.g. FADC250). Helper functions for decoding words and building
 * hit objects are provided as protected utility methods so that all parsers
 * can reuse them.
 */
class ModuleParser {
public:
    virtual ~ModuleParser() = default;

    /**
     * @brief Parse a raw data block and extract physics events
     *
     * @param data_block             EVIO data block to parse
     * @param rocid                  ROC ID for this data block
     * @param physics_events         Vector to be filled with PhysicsEvent*.
     *                               The parser is responsible for allocating
     *                               PhysicsEvent objects; ownership is handed
     *                               to the caller.
     * @param trigger_data         Metadata (event number, etc.)
     *                             from the trigger bank.
     */
    virtual void parse(
        std::shared_ptr<evio::BaseStructure>,
        std::uint32_t,
        std::vector<PhysicsEvent*>&,
        TriggerData&) {
        throw JException("Module parser does not implement parse()");
    }

    /**
     * Context-rich entry point for module parsers which need EVIO bank
     * metadata. The default implementation preserves compatibility with
     * parsers implementing the ROC-ID-only overload.
     */
    virtual void parse(
        std::shared_ptr<evio::BaseStructure> data_block,
        const BankContext& context,
        std::vector<PhysicsEvent*>& physics_events,
        TriggerData& trigger_data) {
        parse(
            std::move(data_block), context.rocid, physics_events, trigger_data);
    }

    /**
     * @brief Set the logger
     *
     * @param logger Reference to the jana::JLogger
     */
    void SetLogger(JLogger& logger) { m_logger = &logger; }

    /**
     * @brief Get the logger
     *
     * @return Reference to the jana::JLogger
     */
    JLogger& GetLogger() const { return *m_logger; }

protected:
    static std::uint32_t getBitsInRange(
        std::uint32_t value,
        int high,
        int low) {
        return (value >> low) & ((1u << (high - low + 1)) - 1);
    }

private:
    JLogger* m_logger = nullptr;
};
