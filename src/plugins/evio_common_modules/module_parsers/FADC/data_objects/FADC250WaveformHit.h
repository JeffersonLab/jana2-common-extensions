#ifndef FADC250WAVEFORM_H
#define FADC250WAVEFORM_H

#include "FADC250Hit.h"
#include <vector>

/**
 * @class FADC250WaveformHit
 * @brief FADC250 waveform hit with raw waveform sample data
 */
class FADC250WaveformHit : public FADC250Hit {
public:
    std::vector<uint32_t> waveform;     ///< Vector of raw ADC waveform samples

    /**
     * @brief Default constructor
     * 
     * Initializes the base hit with default values.
     */
    FADC250WaveformHit() : FADC250Hit() {}
    
    /**
     * @brief Parameterized constructor
     * 
     * Initializes the base hit information with the provided values.
     * 
     * @param trigger_num Trigger number for this hit
     * @param timestamp1 First timestamp value
     * @param timestamp2 Second timestamp value
     * @param rocid Readout Controller ID
     * @param slot Slot number in the crate
     * @param module_id Module identifier
     * @param chan Channel number
     */
    FADC250WaveformHit(uint64_t trigger_num, uint64_t timestamp1, uint64_t timestamp2,
                uint32_t rocid, uint32_t slot, uint32_t module_id, uint32_t chan)
        : FADC250Hit(trigger_num, timestamp1, timestamp2, rocid, slot, module_id, chan) {}
    
    /**
     * @brief Add a waveform sample
     * 
     * Adds a single ADC sample to the waveform data.
     * 
     * @param sample ADC sample value to add
     */
    void addSample(uint32_t sample) {
        waveform.push_back(sample);
    }
    
    /**
     * @brief Get the number of waveform samples
     * 
     * @return Number of samples in the waveform
     */
    size_t getWaveformSize() const {
        return waveform.size();
    }
    
    /**
     * @brief Clear all waveform data
     * 
     * Removes all samples from the waveform vector.
     */
    void clearWaveform() {
        waveform.clear();
    }
};

#endif // FADC250WAVEFORM_H
