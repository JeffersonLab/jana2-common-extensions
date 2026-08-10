#pragma once

#include "DetectorAddress.h"
#include "FADC250PulseHit.h"
#include "HMSHodoscopeDigiHit.h"

HMSHodoscopeDigiHit MakeHMSHodoscopeDigiHit(
    const FADC250PulseHit& pulse,
    const DetectorAddress& address);
