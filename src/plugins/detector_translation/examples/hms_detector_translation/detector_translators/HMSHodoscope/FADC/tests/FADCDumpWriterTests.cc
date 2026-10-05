// Keep regression checks active in Release builds too.
#ifdef NDEBUG
#undef NDEBUG
#endif

#include "FADCDumpWriter.h"
#include "DetectorTranslationDump.h"
#include "FADC250PulseHit.h"
#include "FADC250WaveformHit.h"
#include "FADC250HallBPulseIntegralHit.h"
#include "FADC250HallBPulseTimeHit.h"
#include "FADC250HallBPulsePeakHit.h"

#include <array>
#include <cassert>
#include <sstream>

int main() {
    DetectorTranslationDump dump;
    dump.addWriter<FADC250PulseHit, HMSHodoscopeFADCPulseDigiHit>(
        "HMS_HODOSCOPE", HMSHodoscopeFADCPulseCSVHeader,
        writeHMSHodoscopeFADCPulseCSVRow);
    dump.addWriter<FADC250WaveformHit, HMSHodoscopeFADCWaveformDigiHit>(
        "HMS_HODOSCOPE", HMSHodoscopeFADCWaveformCSVHeader,
        writeHMSHodoscopeFADCWaveformCSVRow);
    dump.addWriter<FADC250HallBPulseIntegralHit, HMSHodoscopeFADCPulseIntegralDigiHit>(
        "HMS_HODOSCOPE", HMSHodoscopeFADCPulseIntegralCSVHeader,
        writeHMSHodoscopeFADCPulseIntegralCSVRow);
    dump.addWriter<FADC250HallBPulseTimeHit, HMSHodoscopeFADCPulseTimeDigiHit>(
        "HMS_HODOSCOPE", HMSHodoscopeFADCPulseTimeCSVHeader,
        writeHMSHodoscopeFADCPulseTimeCSVRow);
    dump.addWriter<FADC250HallBPulsePeakHit, HMSHodoscopeFADCPulsePeakDigiHit>(
        "HMS_HODOSCOPE", HMSHodoscopeFADCPulsePeakCSVHeader,
        writeHMSHodoscopeFADCPulsePeakCSVRow);
    const auto& writers = dump.outputs();
    assert(writers.size() == 5);
    JEvent event;
    auto* pulse = new HMSHodoscopeFADCPulseDigiHit {};
    pulse->rocid = 1;
    pulse->slot = 2;
    pulse->channel = 3;
    pulse->module_id = 4;
    pulse->plane = 5;
    pulse->bar = 6;
    pulse->signal = 7;
    pulse->trigger_num = 8;
    pulse->timestamp1 = 9;
    pulse->timestamp2 = 10;
    pulse->pedestal_quality = 11;
    pulse->pedestal_sum = 12;
    pulse->integral_sum = 13;
    pulse->integral_quality = 14;
    pulse->nsamples_above_threshold = 15;
    pulse->coarse_time = 16;
    pulse->fine_time = 17;
    pulse->pulse_peak = 18;
    pulse->time_quality = 19;
    event.Insert(pulse);
    auto* waveform = new HMSHodoscopeFADCWaveformDigiHit {};
    waveform->rocid = 1;
    waveform->slot = 2;
    waveform->channel = 3;
    waveform->module_id = 4;
    waveform->plane = 5;
    waveform->bar = 6;
    waveform->signal = 7;
    waveform->trigger_num = 8;
    waveform->timestamp1 = 9;
    waveform->timestamp2 = 10;
    waveform->waveform = {101, 102};
    event.Insert(waveform);
    auto* pulseIntegral = new HMSHodoscopeFADCPulseIntegralDigiHit {};
    pulseIntegral->rocid = 1;
    pulseIntegral->slot = 2;
    pulseIntegral->channel = 3;
    pulseIntegral->module_id = 4;
    pulseIntegral->plane = 5;
    pulseIntegral->bar = 6;
    pulseIntegral->signal = 7;
    pulseIntegral->trigger_num = 8;
    pulseIntegral->timestamp1 = 9;
    pulseIntegral->timestamp2 = 10;
    pulseIntegral->pulse_number = 11;
    pulseIntegral->pulse_integral = 12;
    event.Insert(pulseIntegral);
    auto* pulseTime = new HMSHodoscopeFADCPulseTimeDigiHit {};
    pulseTime->rocid = 1;
    pulseTime->slot = 2;
    pulseTime->channel = 3;
    pulseTime->module_id = 4;
    pulseTime->plane = 5;
    pulseTime->bar = 6;
    pulseTime->signal = 7;
    pulseTime->trigger_num = 8;
    pulseTime->timestamp1 = 9;
    pulseTime->timestamp2 = 10;
    pulseTime->pulse_number = 11;
    pulseTime->measurement_quality_factor = 12;
    pulseTime->coarse_pulse_time = 13;
    pulseTime->fine_pulse_time = 14;
    event.Insert(pulseTime);
    auto* pulsePeak = new HMSHodoscopeFADCPulsePeakDigiHit {};
    pulsePeak->rocid = 1;
    pulsePeak->slot = 2;
    pulsePeak->channel = 3;
    pulsePeak->module_id = 4;
    pulsePeak->plane = 5;
    pulsePeak->bar = 6;
    pulsePeak->signal = 7;
    pulsePeak->trigger_num = 8;
    pulsePeak->timestamp1 = 9;
    pulsePeak->timestamp2 = 10;
    pulsePeak->pulse_number = 11;
    pulsePeak->minimum_voltage = 12;
    pulsePeak->peak_voltage = 13;
    event.Insert(pulsePeak);
    const std::array<std::string, 5> filenames {
        "FADC250PulseHit.csv",
        "FADC250WaveformHit.csv",
        "FADC250HallBPulseIntegralHit.csv",
        "FADC250HallBPulseTimeHit.csv",
        "FADC250HallBPulsePeakHit.csv"
    };
    const std::array<std::string, 5> headers {
        "event,rocid,slot,channel,module_id,plane,bar,signal,trigger_num,timestamp1,timestamp2,pedestal_quality,pedestal_sum,integral_sum,integral_quality,nsamples_above_threshold,coarse_time,fine_time,pulse_peak,time_quality",
        "event,rocid,slot,channel,module_id,plane,bar,signal,trigger_num,timestamp1,timestamp2,waveform",
        "event,rocid,slot,channel,module_id,plane,bar,signal,trigger_num,timestamp1,timestamp2,pulse_number,pulse_integral",
        "event,rocid,slot,channel,module_id,plane,bar,signal,trigger_num,timestamp1,timestamp2,pulse_number,measurement_quality_factor,coarse_pulse_time,fine_pulse_time",
        "event,rocid,slot,channel,module_id,plane,bar,signal,trigger_num,timestamp1,timestamp2,pulse_number,minimum_voltage,peak_voltage"
    };
    const std::array<std::string, 5> rows {
        "42,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19\n",
        "42,1,2,3,4,5,6,7,8,9,10,101|102\n",
        "42,1,2,3,4,5,6,7,8,9,10,11,12\n",
        "42,1,2,3,4,5,6,7,8,9,10,11,12,13,14\n",
        "42,1,2,3,4,5,6,7,8,9,10,11,12,13\n"
    };
    JEvent empty;
    for (std::size_t index = 0; index < writers.size(); ++index) {
        assert(writers[index].detector == "HMS_HODOSCOPE");
        assert(writers[index].filename == filenames[index]);
        assert(writers[index].header == headers[index]);
        std::ostringstream missing;
        writers[index].writeEvent(missing, empty);
        assert(missing.str().empty());
    }
    {
        std::ostringstream output;
        writeHMSHodoscopeFADCPulseCSVRow(output, 42, *pulse);
        assert(output.str() == rows[0]);
    }
    {
        std::ostringstream output;
        writeHMSHodoscopeFADCWaveformCSVRow(output, 42, *waveform);
        assert(output.str() == rows[1]);
    }
    {
        std::ostringstream output;
        writeHMSHodoscopeFADCPulseIntegralCSVRow(output, 42, *pulseIntegral);
        assert(output.str() == rows[2]);
    }
    {
        std::ostringstream output;
        writeHMSHodoscopeFADCPulseTimeCSVRow(output, 42, *pulseTime);
        assert(output.str() == rows[3]);
    }
    {
        std::ostringstream output;
        writeHMSHodoscopeFADCPulsePeakCSVRow(output, 42, *pulsePeak);
        assert(output.str() == rows[4]);
    }
}
