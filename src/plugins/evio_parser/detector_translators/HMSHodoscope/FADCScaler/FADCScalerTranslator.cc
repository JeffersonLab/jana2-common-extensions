#include "FADCScalerTranslator.h"

#include <JANA/JEvent.h>

#include <algorithm>
#include <iterator>

#include "HMSHodoscopeIdentity.h"

HMSHodoscopeFADCScalerDigiHit makeHMSHodoscopeFADCScalerDigiHit(
    const FADCScalerHit& scaler,
    const DetectorAddress& address) {
    const auto detector = getHMSHodoscopeIdentity(address);

    HMSHodoscopeFADCScalerDigiHit hit {
        detector.plane,
        detector.bar,
        detector.signal,
        scaler.rocid,
        scaler.slot,
        scaler.words_idx,
        scaler.ncounts,
        {}
    };
    std::copy(std::begin(scaler.counts), std::end(scaler.counts),
        hit.counts.begin());
    return hit;
}

void translateHMSHodoscopeFADCScalerHit(
    const FADCScalerHit& scaler,
    const DetectorAddress& address,
    const JEvent& event) {
    event.Insert(new HMSHodoscopeFADCScalerDigiHit(
        makeHMSHodoscopeFADCScalerDigiHit(scaler, address)));
}
