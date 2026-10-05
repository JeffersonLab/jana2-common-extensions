#include "HMSHodoscopeIdentity.h"

#include <JANA/JException.h>

#include <string>

namespace {

std::int32_t requireField(const DetectorAddress& address, const std::string& name) {
    for (const auto& [field_name, value] : address.fields) {
        if (field_name == name) {
            return value;
        }
    }
    throw JException(
        "%s detector address is missing required field '%s'",
        HMS_HODOSCOPE_DETECTOR_NAME, name.c_str());
}

} // namespace

HMSHodoscopeIdentity getHMSHodoscopeIdentity(
    const DetectorAddress& address) {
    if (address.detector != HMS_HODOSCOPE_DETECTOR_NAME) {
        throw JException(
            "Cannot create HMSHodoscopeIdentity from detector '%s'",
            address.detector.c_str());
    }

    return {
        requireField(address, "plane"),
        requireField(address, "bar"),
        requireField(address, "signal")
    };
}
