#
# CMake configuration for core parser utilities
#

add_library(core STATIC
    EvioEventParser.cc
)

target_link_libraries(core
    PUBLIC
        detector_mapping  # Immutable DAQ-to-detector translation table
        services          # Service utilities
        evio::eviocc      # EVIO C++ library for data I/O
        ${JANA_LIBRARY}   # JANA2 framework library
)

target_include_directories(core
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/data_objects
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/detector_mapping_objects
)

set(CORE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data_objects PARENT_SCOPE)

file(GLOB CORE_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/data_objects/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/detector_mapping_objects/*.h
)

set(CORE_PUBLIC_HEADERS ${CORE_PUBLIC_HEADERS} PARENT_SCOPE)

if(BUILD_TESTING)
    add_executable(translation_table_tests
        detector_mapping_objects/TranslationTableTests.cc
    )
    target_link_libraries(translation_table_tests
        PRIVATE core
    )
    add_test(
        NAME translation_table_tests
        COMMAND translation_table_tests
            ${CMAKE_SOURCE_DIR}/config/evio_parser/detector_mappings/hms_hodoscope.map
    )
endif()
