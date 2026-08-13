#include <cassert>

#include <JANA/JEvent.h>

#include "FADCScalerTranslator.h"

int main() {
    FADCScalerHit scaler(7, 8, 9);
    scaler.ncounts = 16;
    for (std::uint32_t index = 0; index < 16; ++index) {
        scaler.counts[index] = 100 + index;
    }

    const DetectorAddress address {"HMS_HODOSCOPE", {}};
    const auto hit = makeHMSHodoscopeFADCScalerDigiHit(scaler, address);

    assert(hit.rocid == 7);
    assert(hit.slot == 9);
    assert(hit.words_index == 8);
    assert(hit.number_of_counts == 16);
    for (std::uint32_t index = 0; index < 16; ++index) {
        assert(hit.counts[index] == 100 + index);
    }

    JEvent event;
    translateHMSHodoscopeFADCScalerHit(scaler, address, event);
    const auto inserted_hits = event.Get<HMSHodoscopeFADCScalerDigiHit>();
    assert(inserted_hits.size() == 1);
    assert(inserted_hits.front()->counts[15] == 115);
}
