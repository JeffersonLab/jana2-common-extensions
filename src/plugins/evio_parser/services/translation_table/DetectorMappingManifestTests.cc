#include <cassert>
#include <stdexcept>

#include "DetectorMappingManifest.h"

namespace {

bool throwsFor(const char* path) {
    try {
        loadDetectorMappingManifest(path);
    } catch (const std::runtime_error&) {
        return true;
    }
    return false;
}

} // namespace

int main(int argc, char* argv[]) {
    assert(argc == 4);

    const auto ranges = loadDetectorMappingManifest(argv[1]);
    assert(ranges.size() == 2);
    assert(ranges[0] == DetectorMappingRunRange({1, 99, "commissioning.map"}));
    assert(ranges[1] == DetectorMappingRunRange({200, 299, "production.map"}));

    assert(throwsFor(argv[2]));
    assert(throwsFor(argv[3]));
}
