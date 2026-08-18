#
# CMake configuration for services
#

add_library(services STATIC
    translation_table/DetectorMappingManifest.cc
    JEventService_FilterDB.cc
    JEventService_TranslationTable.cc
)

target_include_directories(services
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}    # Expose services/ headers
        ${JANA_INCLUDE_DIRS}           # JANA2 headers
)

target_link_libraries(services
    PUBLIC
        ${JANA_LIBRARY}                # JANA2 framework library
        detector_mapping                 # TranslationTable
        jce_config                     # Provides jce_config_paths.h to services and their consumers
)

set(SERVICES_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

file(GLOB SERVICES_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

set(SERVICES_PUBLIC_HEADERS ${SERVICES_PUBLIC_HEADERS} PARENT_SCOPE)

if(BUILD_TESTING)
    add_executable(detector_translators_map_tests
        tests/JEventService_DetectorTranslatorsMapTests.cc
    )
    target_link_libraries(detector_translators_map_tests PRIVATE services)
    add_test(
        NAME detector_translators_map_tests
        COMMAND detector_translators_map_tests
    )

    add_executable(translation_table_service_tests
        tests/JEventService_TranslationTableTests.cc
    )
    target_link_libraries(translation_table_service_tests
        PRIVATE services
    )
    add_test(
        NAME translation_table_service_tests
        COMMAND translation_table_service_tests
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/testdata
    )

    add_executable(detector_mapping_manifest_tests
        translation_table/tests/DetectorMappingManifestTests.cc
    )
    target_link_libraries(detector_mapping_manifest_tests PRIVATE services)
    add_test(
        NAME detector_mapping_manifest_tests
        COMMAND detector_mapping_manifest_tests
            ${CMAKE_CURRENT_SOURCE_DIR}/translation_table/tests/testdata
    )
endif()
