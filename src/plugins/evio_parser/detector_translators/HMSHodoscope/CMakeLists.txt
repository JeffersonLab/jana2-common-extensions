add_library(hms_hodoscope_translator STATIC
    HMSHodoscopeTranslator.cc
)

target_include_directories(hms_hodoscope_translator
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(hms_hodoscope_translator
    PUBLIC
        detector_mapping
        fadc_parser
)

set(HMS_HODOSCOPE_TRANSLATOR_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    PARENT_SCOPE
)

set(HMS_HODOSCOPE_TRANSLATOR_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/HMSHodoscopeDigiHit.h
    PARENT_SCOPE
)

if(BUILD_TESTING)
    add_executable(hms_hodoscope_translator_tests
        HMSHodoscopeTranslatorTests.cc
    )
    target_link_libraries(hms_hodoscope_translator_tests
        PRIVATE hms_hodoscope_translator
    )
    add_test(
        NAME hms_hodoscope_translator_tests
        COMMAND hms_hodoscope_translator_tests
    )
endif()
