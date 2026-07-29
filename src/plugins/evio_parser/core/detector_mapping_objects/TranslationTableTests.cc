#include <cassert>

#include "TranslationTable.h"

int main() {
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
}
