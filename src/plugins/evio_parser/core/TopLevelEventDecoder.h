#pragma once

#include <cstdint>
#include <memory>
#include <string_view>

#include <JANA/JEvent.h>
#include <JANA/JLogger.h>

#include "eviocc.h"

struct TopLevelEventContext {
    std::uint16_t tag;
    std::uint8_t number;
    std::uint8_t data_type;
    std::uint64_t record_number;
    std::int32_t run_number;
    JLogger& logger;
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
