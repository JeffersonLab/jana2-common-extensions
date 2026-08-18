#include <cassert>
#include <cstdint>
#include <filesystem>
#include <stdexcept>
#include <string>

#include "JEventService_TranslationTable.h"

namespace {

bool rejectsMismatchedDetector(const std::filesystem::path& mapping_directory) {
    try {
        JEventService_TranslationTable service(mapping_directory.string());
        service.Init();
    } catch (const std::runtime_error& error) {
        return std::string(error.what()).find(
            "does not match catalog detector 'HMS_HODOSCOPE'") != std::string::npos;
    }
    return false;
}

bool rejectsPath(
    const std::filesystem::path& mapping_directory,
    const char* expected_message) {
    try {
        JEventService_TranslationTable service(mapping_directory.string());
        service.Init();
    } catch (const std::runtime_error& error) {
        return std::string(error.what()).find(expected_message) != std::string::npos;
    }
    return false;
}

bool rejectsDuplicateAddress(const std::filesystem::path& mapping_directory) {
    try {
        JEventService_TranslationTable service(mapping_directory.string());
        service.Init();
    } catch (const std::runtime_error& error) {
        return std::string(error.what()).find(
            "Duplicate DAQ address while loading detector mapping") != std::string::npos;
    }
    return false;
}

bool rejectsRun(
    const JEventService_TranslationTable& service,
    std::uint64_t run_number) {
    try {
        service.getTable(run_number);
    } catch (const std::runtime_error& error) {
        return std::string(error.what()).find(
            "No detector translation table for run " + std::to_string(run_number)) !=
            std::string::npos;
    }
    return false;
}

} // namespace

int main(int argc, char* argv[]) {
    assert(argc == 2);

    const std::filesystem::path testdata(argv[1]);

    JEventService_TranslationTable service((testdata / "detector_mappings").string());
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

    assert(rejectsMismatchedDetector(testdata / "mismatched_detector_mappings"));
    assert(rejectsPath(
        testdata / "absolute_catalog_path",
        "Detector manifest path must be relative"));
    assert(rejectsPath(
        testdata / "escaping_catalog_path",
        "Detector manifest path must not contain '..'"));
    assert(rejectsPath(
        testdata / "absolute_mapping_path",
        "Mapping file path must be relative"));
    assert(rejectsPath(
        testdata / "escaping_mapping_path",
        "Mapping file path must not contain '..'"));

    JEventService_TranslationTable global_gap_service(
        (testdata / "global_run_gap").string());
    global_gap_service.Init();
    const auto& before_gap = global_gap_service.getTable(50);
    assert(rejectsRun(global_gap_service, 150));
    assert(&global_gap_service.getTable(50) == &before_gap);
    const auto& after_gap = global_gap_service.getTable(250);
    assert(&before_gap != &after_gap);
    assert(&global_gap_service.getTable(50) == &before_gap);
    assert(&global_gap_service.getTable(250) == &after_gap);
    assert(&global_gap_service.getTable(50) == &before_gap);

    JEventService_TranslationTable detector_gap_service(
        (testdata / "detector_run_gap").string());
    detector_gap_service.Init();
    const auto& detector_gap = detector_gap_service.getTable(150);
    assert(detector_gap.Lookup({1, 3, 0}) == nullptr);
    assert(detector_gap.Lookup({2, 4, 1}) != nullptr);

    assert(rejectsDuplicateAddress(testdata / "duplicate_active_address"));
}
