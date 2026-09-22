#pragma once

#include <concepts>

#include "DAQAddress.h"

template <typename HitT>
concept DAQAddressable = requires(const HitT& hit) {
    { getDAQAddress(hit) } -> std::same_as<DAQAddress>;
};
