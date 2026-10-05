#include "FADCScalerDumpWriter.h"

#include <sstream>

namespace {

template <typename RangeT>
std::string join(const RangeT& values) {
    std::ostringstream output;
    for (std::size_t index = 0; index < values.size(); ++index) {
        if (index != 0) {
            output << '|';
        }
        output << values[index];
    }
    return output.str();
}

} // namespace

const std::string HMSHodoscopeFADCScalerCSVHeader = "event,rocid,slot,plane,bar,signal,words_index,number_of_counts,counts";

void writeHMSHodoscopeFADCScalerCSVRow(
    std::ostream& output, std::uint64_t event_number,
    const HMSHodoscopeFADCScalerDigiHit& hit) {
    output
        << event_number << ','
        << hit.rocid << ','
        << hit.slot << ','
        << hit.plane << ','
        << hit.bar << ','
        << hit.signal << ','
        << hit.words_index << ','
        << hit.number_of_counts << ','
        << join(hit.counts) << '\n';
}
