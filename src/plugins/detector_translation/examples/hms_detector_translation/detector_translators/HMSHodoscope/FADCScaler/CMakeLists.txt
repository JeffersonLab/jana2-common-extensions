add_library(hms_hodoscope_fadc_scaler_translator STATIC
    FADCScalerTranslator.cc
)

target_include_directories(hms_hodoscope_fadc_scaler_translator
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/data_objects
)

target_link_libraries(hms_hodoscope_fadc_scaler_translator
    PUBLIC
        detector_mapping
        fadc_scaler_parser
        hms_hodoscope_identity
)

set(HMS_HODOSCOPE_FADC_SCALER_TRANSLATOR_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/data_objects
    PARENT_SCOPE
)

set(HMS_HODOSCOPE_FADC_SCALER_TRANSLATOR_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/data_objects/FADCScalerDigiHit.h
    PARENT_SCOPE
)

if(BUILD_TESTING)
    add_executable(hms_hodoscope_fadc_scaler_translator_tests
        tests/FADCScalerTranslatorTests.cc
    )
    target_link_libraries(hms_hodoscope_fadc_scaler_translator_tests
        PRIVATE hms_hodoscope_fadc_scaler_translator
    )
    add_test(
        NAME hms_hodoscope_fadc_scaler_translator_tests
        COMMAND hms_hodoscope_fadc_scaler_translator_tests
    )
endif()
