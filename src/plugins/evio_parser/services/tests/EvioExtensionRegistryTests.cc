#include <JANA/JException.h>

#include <cassert>
#include <memory>
#include <string_view>

#include "JEventService_BankToModuleMap.h"
#include "JEventService_TopLevelEventDecoders.h"

namespace {

class Decoder final : public TopLevelEventDecoder {
public:
    explicit Decoder(std::string_view decoder_name) : m_name(decoder_name) {}

    std::string_view name() const noexcept override { return m_name; }

    void decode(
        const std::shared_ptr<evio::EvioEvent>&,
        const TopLevelEventContext&,
        JEvent&) const override {}

private:
    std::string_view m_name;
};

} // namespace

int main() {
    JEventService_BankToModuleMap banks;
    banks.addRoute(0xace, 0x100ace);
    assert(banks.getParserId(0xace) == 0x100ace);

    bool frozen_bank_map_rejected = false;
    try {
        banks.addRoute(0xacf, 0x100acf);
    }
    catch (const JException&) {
        frozen_bank_map_rejected = true;
    }
    assert(frozen_bank_map_rejected);

    JEventService_TopLevelEventDecoders decoders;
    decoders.addDecoder(
        "family",
        {.tag_value = 0x0a00, .tag_mask = 0x0f00},
        JEventLevel::SlowControls,
        std::make_shared<Decoder>("family"));
    decoders.addDecoder(
        "exact",
        {.tag_value = 0xace, .tag_mask = 0xffff, .number = 7},
        JEventLevel::SlowControls,
        std::make_shared<Decoder>("exact"));

    const auto exact = decoders.resolve(0xace, 7, 0x10);
    assert(exact && exact->key == "exact");

    const auto family = decoders.resolve(0xab0, 1, 0x10);
    assert(family && family->key == "family");

    bool frozen_decoder_map_rejected = false;
    try {
        decoders.addDecoder(
            "late",
            {.tag_value = 1, .tag_mask = 0xffff},
            JEventLevel::SlowControls,
            std::make_shared<Decoder>("late"));
    }
    catch (const JException&) {
        frozen_decoder_map_rejected = true;
    }
    assert(frozen_decoder_map_rejected);

    JEventService_TopLevelEventDecoders ambiguous;
    ambiguous.addDecoder(
        "one",
        {.tag_value = 0xa00, .tag_mask = 0xf00},
        JEventLevel::SlowControls,
        std::make_shared<Decoder>("one"));
    ambiguous.addDecoder(
        "two",
        {.tag_value = 0x0c0, .tag_mask = 0x0f0},
        JEventLevel::SlowControls,
        std::make_shared<Decoder>("two"));

    bool ambiguity_rejected = false;
    try {
        static_cast<void>(ambiguous.resolve(0xac0, 0, 0));
    }
    catch (const JException&) {
        ambiguity_rejected = true;
    }
    assert(ambiguity_rejected);
}
