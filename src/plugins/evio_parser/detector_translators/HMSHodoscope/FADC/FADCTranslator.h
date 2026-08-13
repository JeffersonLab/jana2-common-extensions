#pragma once

#include "DetectorAddress.h"
#include "FADC250PulseHit.h"
#include "FADCDigiHit.h"

class JEvent;

HMSHodoscopeFADCDigiHit makeHMSHodoscopeFADCDigiHit(
    const FADC250PulseHit& pulse,
    const DetectorAddress& address);

void translateHMSHodoscopeFADCHit(
    const FADC250PulseHit& pulse,
    const DetectorAddress& address,
    const JEvent& event);
