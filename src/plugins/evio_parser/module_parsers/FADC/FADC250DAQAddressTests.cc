#include <cassert>

#include "DAQAddressable.h"
#include "FADC250PulseHit.h"

namespace {

struct NonAddressableHit {};

static_assert(DAQAddressable<FADC250Hit>);
static_assert(DAQAddressable<FADC250PulseHit>);
static_assert(!DAQAddressable<NonAddressableHit>);

} // namespace

int main() {
    FADC250PulseHit hit;
    hit.rocid = 7;
    hit.slot = 8;
    hit.chan = 9;

    const DAQAddress expected {7, 8, 9};
    assert(getDAQAddress(hit) == expected);
}
