add_library(detector_translation_services STATIC
    translation_table/DetectorMappingManifest.cc
    JEventService_TranslationTable.cc
)

target_include_directories(detector_translation_services
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/translation_table>
        $<INSTALL_INTERFACE:include/${INSTALL_NAMESPACE}/plugins/detector_translation>
)

target_link_libraries(detector_translation_services
    PUBLIC
        ${JANA_LIBRARY}
        detector_mapping
        evio_parser_api
        jce_config
)

install(TARGETS detector_translation_services
    EXPORT jana2_common_extensionsTargets
    ARCHIVE DESTINATION lib/${INSTALL_NAMESPACE}
)

set(DETECTOR_TRANSLATION_SERVICES_INCLUDE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/translation_table
    PARENT_SCOPE
)
file(GLOB DETECTOR_TRANSLATION_SERVICES_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/translation_table/*.h
)
set(DETECTOR_TRANSLATION_SERVICES_PUBLIC_HEADERS
    ${DETECTOR_TRANSLATION_SERVICES_PUBLIC_HEADERS}
    PARENT_SCOPE
)

if(BUILD_TESTING)
    add_executable(detector_mapping_catalogs_tests
        tests/JEventService_DetectorMappingCatalogsTests.cc
    )
    target_link_libraries(detector_mapping_catalogs_tests
        PRIVATE detector_translation_services
    )
    add_test(
        NAME detector_mapping_catalogs_tests
        COMMAND detector_mapping_catalogs_tests
    )

    add_executable(detector_translators_map_tests
        tests/JEventService_DetectorTranslatorsMapTests.cc
    )
    target_link_libraries(detector_translators_map_tests
        PRIVATE detector_translation_services
    )
    add_test(
        NAME detector_translators_map_tests
        COMMAND detector_translators_map_tests
    )

    add_executable(translation_table_service_tests
        tests/JEventService_TranslationTableTests.cc
    )
    target_link_libraries(translation_table_service_tests
        PRIVATE detector_translation_services
    )
    add_test(
        NAME translation_table_service_tests
        COMMAND translation_table_service_tests
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/testdata
    )

    add_executable(detector_mapping_manifest_tests
        translation_table/tests/DetectorMappingManifestTests.cc
    )
    target_link_libraries(detector_mapping_manifest_tests
        PRIVATE detector_translation_services
    )
    add_test(
        NAME detector_mapping_manifest_tests
        COMMAND detector_mapping_manifest_tests
            ${CMAKE_CURRENT_SOURCE_DIR}/translation_table/tests/testdata
    )
endif()
