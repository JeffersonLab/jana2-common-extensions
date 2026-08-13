#include "FADCScalerTranslator.h"

#include <JANA/JEvent.h>
#include <JANA/JException.h>

#include <algorithm>
#include <iterator>

HMSHodoscopeFADCScalerDigiHit makeHMSHodoscopeFADCScalerDigiHit(
    const FADCScalerHit& scaler,
    const DetectorAddress& address) {
    if (address.detector != "HMS_HODOSCOPE") {
        throw JException(
            "Cannot create HMSHodoscopeFADCScalerDigiHit from detector '%s'",
            address.detector.c_str());
    }

    HMSHodoscopeFADCScalerDigiHit hit {
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
