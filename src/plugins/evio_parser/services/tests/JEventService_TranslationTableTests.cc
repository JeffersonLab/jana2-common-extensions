#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <string>

#include "JEventService_TranslationTable.h"

namespace {

bool rejectsMismatchedDetector(const char* mapping_directory) {
    try {
        JEventService_TranslationTable service(mapping_directory);
        service.Init();
    } catch (const std::runtime_error& error) {
        return std::string(error.what()).find(
            "does not match catalog detector 'HMS_HODOSCOPE'") != std::string::npos;
    }
    return false;
}

bool rejectsPath(const char* mapping_directory, const char* expected_message) {
    try {
        JEventService_TranslationTable service(mapping_directory);
        service.Init();
    } catch (const std::runtime_error& error) {
        return std::string(error.what()).find(expected_message) != std::string::npos;
    }
    return false;
}

} // namespace

int main(int argc, char* argv[]) {
    assert(argc == 7);

    JEventService_TranslationTable service(argv[1]);
    service.Init();

    const auto& first_run = service.getTable(100);
    const auto& same_mapping_run = service.getTable(175);
    const auto& second_run = service.getTable(250);
    const auto& cached_second_run = service.getTable(275);

    assert(&first_run == &same_mapping_run);
    assert(&first_run != &second_run);
    assert(&second_run == &cached_second_run);

    const DetectorAddress expected_hms {
        "HMS_HODOSCOPE",
        {{"plane", 1}, {"bar", 1}, {"signal", 0}}
    };
    const auto* hms_address = first_run.Lookup({1, 3, 0});
    assert(hms_address != nullptr);
    assert(*hms_address == expected_hms);

    const DetectorAddress expected_current_hms {
        "HMS_HODOSCOPE",
        {{"plane", 9}, {"bar", 8}, {"signal", 7}}
    };
    const auto* current_hms_address = second_run.Lookup({1, 3, 0});
    assert(current_hms_address != nullptr);
    assert(*current_hms_address == expected_current_hms);

    const DetectorAddress expected_bcal {
        "BCAL",
        {{"module", 2}, {"layer", 3}, {"sector", 4}, {"end", 0}}
    };
    const auto* bcal_address = first_run.Lookup({2, 4, 1});
    assert(bcal_address != nullptr);
    assert(*bcal_address == expected_bcal);
    const auto* current_bcal_address = second_run.Lookup({2, 4, 1});
    assert(current_bcal_address != nullptr);
    assert(*current_bcal_address == expected_bcal);

    assert(rejectsMismatchedDetector(argv[2]));
    assert(rejectsPath(argv[3], "Detector manifest path must be relative"));
    assert(rejectsPath(argv[4], "Detector manifest path must not contain '..'"));
    assert(rejectsPath(argv[5], "Mapping file path must be relative"));
    assert(rejectsPath(argv[6], "Mapping file path must not contain '..'"));
}
