// Keep regression checks active in Release builds too.
#ifdef NDEBUG
#undef NDEBUG
#endif

#include "JEventService_DetectorTranslatorsMap.h"

#include <cassert>
#include <sstream>

struct RawHit { std::uint32_t channel; int value; };
struct OtherRawHit { std::uint32_t channel; int value; };
struct DigiHit { int value; };

DAQAddress getDAQAddress(const RawHit& hit) { return {1, 2, hit.channel}; }
DAQAddress getDAQAddress(const OtherRawHit& hit) { return {1, 2, hit.channel}; }

void writeRow(std::ostream& output, std::uint64_t event_number, const DigiHit& hit) {
    output << event_number << ',' << hit.value << '\n';
}

int main() {
    JEventService_DetectorTranslatorsMap translators;
    auto convert = [](const RawHit& raw, const DetectorAddress&) {
        return DigiHit {raw.value};
    };
    translators.addTranslator<RawHit, DigiHit>("A", convert, "event,value", writeRow);
    translators.addTranslator<RawHit, DigiHit>("B", convert, "event,value", writeRow);
    translators.addTranslator<OtherRawHit, DigiHit>("A",
        [](const OtherRawHit& raw, const DetectorAddress&) {
            return DigiHit {raw.value};
        }, "event,value", writeRow);
    assert(translators.getTranslatorCount<RawHit>() == 2);
    assert(translators.getTranslatorCount<OtherRawHit>() == 1);
    try {
        translators.addTranslator<RawHit, DigiHit>("A", convert, "event,value", writeRow);
        assert(false);
    } catch (const JException&) {}
    try {
        translators.addTranslator<RawHit, DigiHit>("C", convert, "event,value", {});
        assert(false);
    } catch (const JException&) {}
    try {
        translators.addTranslator<RawHit, DigiHit>("", convert, "event,value", writeRow);
        assert(false);
    } catch (const JException&) {}
    try {
        translators.addTranslator<RawHit, DigiHit>("C", {}, "event,value", writeRow);
        assert(false);
    } catch (const JException&) {}
    try {
        translators.addTranslator<RawHit, DigiHit>("C", convert, "", writeRow);
        assert(false);
    } catch (const JException&) {}
    assert(DetectorTranslationDump::pathComponent("../%/") ==
        "%2E%2E%2F%25%2F");
    DetectorTranslationDump dump;
    dump.addWriter<RawHit, DigiHit>("A", "event,value", writeRow);
    try {
        dump.addWriter<RawHit, DigiHit>("A", "event,value", writeRow);
        assert(false);
    } catch (const JException&) {}
    assert(dump.outputs().size() == 1);
    const auto outputs = translators.getDumpOutputs();
    assert(outputs.size() == 3);
    assert(outputs[0].detector == "A" && outputs[0].filename == "RawHit.csv");
    assert(outputs[1].detector == "B" && outputs[1].filename == "RawHit.csv");
    assert(outputs[2].detector == "A" && outputs[2].filename == "OtherRawHit.csv");
    try {
        translators.addTranslator<RawHit, DigiHit>("C", convert, "event,value", writeRow);
        assert(false);
    } catch (const JException&) {}

    TranslationTable table;
    assert(table.Insert({1, 2, 3}, {"A", {}}));
    assert(table.Insert({1, 2, 4}, {"B", {}}));
    assert(table.Insert({1, 2, 5}, {"C", {}}));
    JEvent event;
    event.SetEventNumber(42);
    event.Insert(new RawHit {3, 7});
    event.Insert(new RawHit {3, 70});
    event.Insert(new RawHit {4, 8});
    event.Insert(new RawHit {5, 90}); // Mapped detector with no route: skipped.
    event.Insert(new RawHit {99, 100}); // Unmapped: skipped.
    event.Insert(new OtherRawHit {3, 9});
    event.Insert(new DigiHit {999}); // Not produced by a route: excluded from dumps.
    translators.translateEvent(table, event);
    assert(event.Get<DigiHit>().size() == 5);
    const std::vector<std::string> rows {"42,7\n42,70\n", "42,8\n", "42,9\n"};
    for (std::size_t i = 0; i < outputs.size(); ++i) {
        std::ostringstream stream;
        outputs[i].writeEvent(stream, event);
        assert(stream.str() == rows[i]);
        assert(outputs[i].header == "event,value");
    }
    JEvent empty;
    translators.translateEvent(table, empty);
    for (const auto& output : outputs) {
        std::ostringstream stream;
        output.writeEvent(stream, empty);
        assert(stream.str().empty());
    }
    // Processing another event must not replace the first event's route outputs.
    std::ostringstream first;
    outputs[0].writeEvent(first, event);
    assert(first.str() == "42,7\n42,70\n");
    JEventService_DetectorTranslatorsMap none;
    none.translateEvent(table, empty);
    try {
        none.addTranslator<RawHit, DigiHit>("A", convert, "event,value", writeRow);
        assert(false);
    } catch (const JException&) {}
    assert(none.getDumpOutputs().empty());
}
