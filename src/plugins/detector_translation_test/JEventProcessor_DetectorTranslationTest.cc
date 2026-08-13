#include "JEventProcessor_DetectorTranslationTest.h"

#include <JANA/JEvent.h>
#include <JANA/JException.h>

JEventProcessor_DetectorTranslationTest::
JEventProcessor_DetectorTranslationTest() {
    SetTypeName("JEventProcessor_DetectorTranslationTest");
    SetPrefix("detector_translation_test");
    SetCallbackStyle(CallbackStyle::ExpertMode);
    m_hits.SetOptional(true);
}

void JEventProcessor_DetectorTranslationTest::Init() {
    m_output.open(m_outputFilename());
    if (!m_output.is_open()) {
        throw JException(
            "Failed to open detector translation test output: %s",
            m_outputFilename().c_str());
    }

    m_output
        << "event,rocid,slot,channel,plane,bar,signal,integral_sum,"
           "pedestal_sum,coarse_time,fine_time,pulse_peak\n";
}

void JEventProcessor_DetectorTranslationTest::ProcessSequential(
    const JEvent& event) {
    for (const auto* hit : m_hits()) {
        m_output
            << event.GetEventNumber() << ','
            << hit->rocid << ','
            << hit->slot << ','
            << hit->channel << ','
            << hit->plane << ','
            << hit->bar << ','
            << hit->signal << ','
            << hit->integral_sum << ','
            << hit->pedestal_sum << ','
            << hit->coarse_time << ','
            << hit->fine_time << ','
            << hit->pulse_peak << '\n';
    }
}

void JEventProcessor_DetectorTranslationTest::Finish() {
    m_output.close();
}
