#include <cassert>
#include <string>

#include "TranslationTable.h"

int main(int argc, char* argv[]) {
    assert(argc == 3);

    TranslationTable table;
    const DetectorAddress hms_address {
        "HMS_HODOSCOPE",
        {{"plane", 1}, {"bar", 1}, {"signal", 0}}
    };

    assert(table.Insert({1, 8, 0}, hms_address));
    assert(!table.Insert({1, 8, 0}, {"OTHER_DETECTOR", {{"channel", 9}}}));

    const auto* mapped = table.Lookup({1, 8, 0});
    assert(mapped != nullptr);
    assert(*mapped == hms_address);

    assert(table.Lookup({1, 8, 1}) == nullptr);

    TranslationTable loaded;
    loaded.LoadMappingFile(argv[1]);

    const auto* loaded_address = loaded.Lookup({1, 3, 0});
    assert(loaded_address != nullptr);
    assert(*loaded_address == hms_address);
    assert(loaded.Lookup({1, 3, 1}) == nullptr);

    TranslationTable board_level;
    board_level.LoadMappingFile(argv[2]);
    const auto* board_address = board_level.Lookup({
        9,
        10,
        DAQAddress::UnspecifiedChannel
    });
    assert(board_address != nullptr);
    assert(board_address->detector == "BOARD_LEVEL_DETECTOR");
}
