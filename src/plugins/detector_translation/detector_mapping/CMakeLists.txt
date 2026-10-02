add_library(detector_mapping STATIC
    TranslationTable.cc
)

target_include_directories(detector_mapping
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include/${INSTALL_NAMESPACE}/plugins/detector_translation>
)

target_link_libraries(detector_mapping PUBLIC evio_parser_api)

install(TARGETS detector_mapping
    EXPORT jana2_common_extensionsTargets
    ARCHIVE DESTINATION lib/${INSTALL_NAMESPACE}
)

set(DETECTOR_MAPPING_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
file(GLOB DETECTOR_MAPPING_PUBLIC_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
set(DETECTOR_MAPPING_PUBLIC_HEADERS
    ${DETECTOR_MAPPING_PUBLIC_HEADERS}
    PARENT_SCOPE
)

if(BUILD_TESTING)
    add_executable(translation_table_tests
        tests/TranslationTableTests.cc
    )
    target_link_libraries(translation_table_tests PRIVATE detector_mapping)
    add_test(
        NAME translation_table_tests
        COMMAND translation_table_tests
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/testdata/hms_hodoscope.map
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/testdata/channel_none.map
    )
endif()
