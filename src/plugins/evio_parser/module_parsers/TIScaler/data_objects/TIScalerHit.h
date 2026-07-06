#ifndef TISCALERHIT_H
#define TISCALERHIT_H

#include <cstdint>

/**
 * @class TIScalerHit
 * @brief Hit class for TI scaler data
 * 
 * This class contains the data members for one TI scaler record.
 */
class TIScalerHit {
public:
    uint32_t rocid;
    uint32_t words_idx;
    uint32_t slot;
    uint32_t nscalerwords;

    // Individual scaler channels:
    uint32_t live_time;                 // 0 Live time
    uint32_t busy_time;                 // 1 Busy time
    uint32_t ts_input_1;                // 2 TS input #1
    uint32_t ts_input_2;                // 3 TS input #2
    uint32_t ts_input_3;                // 4 TS input #3
    uint32_t ts_input_4;                // 5 TS input #4
    uint32_t ts_input_5;                // 6 TS input #5
    uint32_t ts_input_6;                // 7 TS input #6
    uint32_t all_triggers_before_busy;  // 8 All trigger sources, before busy
    uint32_t ts_inputs_before_busy;     // 11 Only TS inputs, before busy
    uint32_t ti_event_number;           // Event number obtained from TI

    /// Default constructor: initializes all members to zero.
    TIScalerHit()
        : rocid(0),
          words_idx(0),
          slot(0),
          nscalerwords(0),
          live_time(0),
          busy_time(0),
          ts_input_1(0),
          ts_input_2(0),
          ts_input_3(0),
          ts_input_4(0),
          ts_input_5(0),
          ts_input_6(0),
          all_triggers_before_busy(0),
          ts_inputs_before_busy(0),
          ti_event_number(0)  {}
};

#endif // TISCALERHIT_H

