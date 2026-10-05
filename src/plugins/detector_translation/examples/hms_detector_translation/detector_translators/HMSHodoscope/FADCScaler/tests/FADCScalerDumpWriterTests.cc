// Keep regression checks active in Release builds too.
#ifdef NDEBUG
#undef NDEBUG
#endif

#include "FADCScalerDumpWriter.h"
#include "DetectorTranslationDump.h"
#include "FADCScalerHit.h"

#include <array>
#include <cassert>
#include <sstream>

int main() {
    DetectorTranslationDump dump;
    dump.addWriter<FADCScalerHit, HMSHodoscopeFADCScalerDigiHit>(
        "HMS_HODOSCOPE", HMSHodoscopeFADCScalerCSVHeader,
        writeHMSHodoscopeFADCScalerCSVRow);
    const auto& writers = dump.outputs();
    assert(writers.size() == 1);
    JEvent event;
    auto* scaler = new HMSHodoscopeFADCScalerDigiHit {};
    scaler->rocid = 1;
    scaler->slot = 2;
    scaler->plane = 3;
    scaler->bar = 4;
    scaler->signal = 5;
    scaler->words_index = 6;
    scaler->number_of_counts = 7;
    for (std::size_t i = 0; i < scaler->counts.size(); ++i) {
        scaler->counts[i] = i + 20;
    }
    event.Insert(scaler);
    const std::array<std::string, 1> filenames {
        "FADCScalerHit.csv"
    };
    const std::array<std::string, 1> headers {
        "event,rocid,slot,plane,bar,signal,words_index,number_of_counts,counts"
    };
    const std::array<std::string, 1> rows {
        "42,1,2,3,4,5,6,7,20|21|22|23|24|25|26|27|28|29|30|31|32|33|34|35\n"
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
        writeHMSHodoscopeFADCScalerCSVRow(output, 42, *scaler);
        assert(output.str() == rows[0]);
    }
}
