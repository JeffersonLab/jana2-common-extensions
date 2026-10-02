#pragma once

#include <cstdint>

#include "DetectorAddress.h"

struct HMSHodoscopeIdentity {
    std::int32_t plane;
    std::int32_t bar;
    std::int32_t signal;
};

HMSHodoscopeIdentity getHMSHodoscopeIdentity(
    const DetectorAddress& address);
