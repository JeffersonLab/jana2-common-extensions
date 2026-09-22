#pragma once

#include "DetectorAddress.h"
#include "FADC250HallBPulseIntegralHit.h"
#include "FADC250HallBPulsePeakHit.h"
#include "FADC250HallBPulseTimeHit.h"
#include "FADC250PulseHit.h"
#include "FADC250WaveformHit.h"
#include "FADCPulseDigiHit.h"
#include "FADCPulseIntegralDigiHit.h"
#include "FADCPulsePeakDigiHit.h"
#include "FADCPulseTimeDigiHit.h"
#include "FADCWaveformDigiHit.h"

class JEvent;

HMSHodoscopeFADCPulseDigiHit makeHMSHodoscopeFADCPulseDigiHit(
    const FADC250PulseHit& pulse,
    const DetectorAddress& address);

HMSHodoscopeFADCWaveformDigiHit makeHMSHodoscopeFADCWaveformDigiHit(
    const FADC250WaveformHit& waveform,
    const DetectorAddress& address);

HMSHodoscopeFADCPulseIntegralDigiHit
makeHMSHodoscopeFADCPulseIntegralDigiHit(
    const FADC250HallBPulseIntegralHit& integral,
    const DetectorAddress& address);

HMSHodoscopeFADCPulseTimeDigiHit makeHMSHodoscopeFADCPulseTimeDigiHit(
    const FADC250HallBPulseTimeHit& time,
    const DetectorAddress& address);

HMSHodoscopeFADCPulsePeakDigiHit makeHMSHodoscopeFADCPulsePeakDigiHit(
    const FADC250HallBPulsePeakHit& peak,
    const DetectorAddress& address);

void translateHMSHodoscopeFADCPulseHit(
    const FADC250PulseHit& pulse,
    const DetectorAddress& address,
    const JEvent& event);

void translateHMSHodoscopeFADCWaveformHit(
    const FADC250WaveformHit& waveform,
    const DetectorAddress& address,
    const JEvent& event);

void translateHMSHodoscopeFADCPulseIntegralHit(
    const FADC250HallBPulseIntegralHit& integral,
    const DetectorAddress& address,
    const JEvent& event);

void translateHMSHodoscopeFADCPulseTimeHit(
    const FADC250HallBPulseTimeHit& time,
    const DetectorAddress& address,
    const JEvent& event);

void translateHMSHodoscopeFADCPulsePeakHit(
    const FADC250HallBPulsePeakHit& peak,
    const DetectorAddress& address,
    const JEvent& event);
