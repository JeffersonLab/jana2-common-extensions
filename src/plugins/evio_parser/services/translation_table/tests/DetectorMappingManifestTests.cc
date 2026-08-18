#include <cassert>
#include <filesystem>
#include <limits>
#include <stdexcept>

#include "translation_table/DetectorMappingManifest.h"

namespace {

bool throwsFor(const std::filesystem::path& path) {
    try {
        loadDetectorMappingManifest(path.string());
    } catch (const std::runtime_error&) {
        return true;
    }
    return false;
}

bool catalogThrowsFor(const std::filesystem::path& path) {
    try {
        loadDetectorMappingCatalog(path.string());
    } catch (const std::runtime_error&) {
        return true;
    }
    return false;
}

} // namespace

int main(int argc, char* argv[]) {
    assert(argc == 2);

    const std::filesystem::path testdata(argv[1]);
    const auto manifests = testdata / "manifests";
    const auto catalogs = testdata / "catalogs";

    const auto ranges = loadDetectorMappingManifest((manifests / "valid.map").string());
    assert(ranges.size() == 4);
    assert(ranges[0] == DetectorMappingRunRange({0, 0, "zero.map"}));
    assert(ranges[1] == DetectorMappingRunRange({1, 99, "commissioning.map"}));
    assert(ranges[2] == DetectorMappingRunRange({200, 299, "production.map"}));
    assert(ranges[3] == DetectorMappingRunRange({
        300,
        std::numeric_limits<std::uint64_t>::max(),
        "current.map"
    }));

    assert(throwsFor(manifests / "overlap.map"));
    assert(throwsFor(manifests / "malformed.map"));

    const auto catalog = loadDetectorMappingCatalog((catalogs / "valid.map").string());
    assert(catalog.size() == 2);
    assert(catalog[0] == DetectorMappingCatalogEntry({
        "BCAL",
        "bcal/manifest.map"
    }));
    assert(catalog[1] == DetectorMappingCatalogEntry({
        "HMS_HODOSCOPE",
        "hms_hodoscope/manifest.map"
    }));

    assert(catalogThrowsFor(catalogs / "duplicate.map"));
    assert(catalogThrowsFor(catalogs / "malformed.map"));
}
