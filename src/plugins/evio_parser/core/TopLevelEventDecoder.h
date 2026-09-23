#pragma once

#include <cstdint>
#include <memory>
#include <string_view>

#include <JANA/JEvent.h>
#include <JANA/JLogger.h>

#include "eviocc.h"

struct TopLevelEventContext {
    std::uint16_t tag;            // Top-level EVIO event header tag.
    std::uint8_t number;          // Top-level EVIO event header number (num).
    std::uint8_t data_type;       // Top-level EVIO event header data type code.
    std::uint64_t record_number;  // EVIO event number, not a trigger-derived physics event number.
    std::int32_t run_number;      // Current JANA run number assigned by the source.
    JLogger& logger;              // Source logger for decoder diagnostics.
};

class TopLevelEventDecoder {
public:
    virtual ~TopLevelEventDecoder() = default;
    virtual std::string_view name() const noexcept = 0;

    virtual void decode(
        const std::shared_ptr<evio::EvioEvent>& raw_event,
        const TopLevelEventContext& context,
        JEvent& output) const = 0;
};
