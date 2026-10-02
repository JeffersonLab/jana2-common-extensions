#pragma once

#include "DetectorAddress.h"
#include "FADCScalerDigiHit.h"
#include "FADCScalerHit.h"

class JEvent;

HMSHodoscopeFADCScalerDigiHit makeHMSHodoscopeFADCScalerDigiHit(
    const FADCScalerHit& scaler,
    const DetectorAddress& address);

void translateHMSHodoscopeFADCScalerHit(
    const FADCScalerHit& scaler,
    const DetectorAddress& address,
    const JEvent& event);
