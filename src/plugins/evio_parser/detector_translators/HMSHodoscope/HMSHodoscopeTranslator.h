#pragma once

#include "DetectorAddress.h"
#include "FADC250PulseHit.h"
#include "HMSHodoscopeDigiHit.h"

class JEvent;

HMSHodoscopeDigiHit makeHMSHodoscopeDigiHit(
    const FADC250PulseHit& pulse,
    const DetectorAddress& address);

void translateHMSHodoscopeFADCHit(
    const FADC250PulseHit& pulse,
    const DetectorAddress& address,
    const JEvent& event);
