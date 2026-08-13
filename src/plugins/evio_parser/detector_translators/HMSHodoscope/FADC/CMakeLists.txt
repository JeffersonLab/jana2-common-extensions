add_library(hms_hodoscope_fadc_translator STATIC
    FADCTranslator.cc
)

target_include_directories(hms_hodoscope_fadc_translator
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/data_objects
)

target_link_libraries(hms_hodoscope_fadc_translator
    PUBLIC
        detector_mapping
        fadc_parser
        hms_hodoscope_identity
)

set(HMS_HODOSCOPE_FADC_TRANSLATOR_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/data_objects
    PARENT_SCOPE
)

set(HMS_HODOSCOPE_FADC_TRANSLATOR_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/data_objects/FADCPulseDigiHit.h
    ${CMAKE_CURRENT_SOURCE_DIR}/data_objects/FADCWaveformDigiHit.h
    ${CMAKE_CURRENT_SOURCE_DIR}/data_objects/FADCPulseIntegralDigiHit.h
    ${CMAKE_CURRENT_SOURCE_DIR}/data_objects/FADCPulseTimeDigiHit.h
    ${CMAKE_CURRENT_SOURCE_DIR}/data_objects/FADCPulsePeakDigiHit.h
    PARENT_SCOPE
)

if(BUILD_TESTING)
    add_executable(hms_hodoscope_fadc_translator_tests
        FADCTranslatorTests.cc
    )
    target_link_libraries(hms_hodoscope_fadc_translator_tests
        PRIVATE hms_hodoscope_fadc_translator
    )
    add_test(
        NAME hms_hodoscope_fadc_translator_tests
        COMMAND hms_hodoscope_fadc_translator_tests
    )
endif()
