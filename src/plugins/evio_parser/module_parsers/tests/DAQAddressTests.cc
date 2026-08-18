#include <cassert>

#include "CAEN1190Hit.h"
#include "DAQAddressable.h"
#include "FADCScalerHit.h"
#include "FADC250PulseHit.h"
#include "HelicityDecoderData.h"
#include "MPDHit.h"
#include "TIScalerHit.h"
#include "VFTDCHit.h"

namespace {

struct NonAddressableHit {};

static_assert(DAQAddressable<FADC250Hit>);
static_assert(DAQAddressable<FADC250PulseHit>);
static_assert(DAQAddressable<CAEN1190Hit>);
static_assert(DAQAddressable<FADCScalerHit>);
static_assert(DAQAddressable<MPDHit>);
static_assert(DAQAddressable<VFTDCHit>);
static_assert(!DAQAddressable<TIScalerHit>);
static_assert(!DAQAddressable<HelicityDecoderData>);
static_assert(!DAQAddressable<NonAddressableHit>);

} // namespace

int main() {
    FADC250PulseHit hit;
    hit.rocid = 7;
    hit.slot = 8;
    hit.chan = 9;

    assert(getDAQAddress(hit) == DAQAddress({7, 8, 9}));

    CAEN1190Hit caen;
    caen.rocid = 1;
    caen.slot = 2;
    caen.chan = 3;
    assert(getDAQAddress(caen) == DAQAddress({1, 2, 3}));

    VFTDCHit vftdc;
    vftdc.rocid = 4;
    vftdc.slot = 5;
    vftdc.channel_num = 6;
    assert(getDAQAddress(vftdc) == DAQAddress({4, 5, 6}));

    MPDHit mpd;
    mpd.rocid = 10;
    mpd.slot = 11;
    mpd.apv_channel = 12;
    assert(getDAQAddress(mpd) == DAQAddress({10, 11, 12}));

    FADCScalerHit fadc_scaler;
    fadc_scaler.rocid = 13;
    fadc_scaler.slot = 14;
    assert(getDAQAddress(fadc_scaler) == DAQAddress({
        13,
        14,
        DAQAddress::UnspecifiedChannel
    }));

}
