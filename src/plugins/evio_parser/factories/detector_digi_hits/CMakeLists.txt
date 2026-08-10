add_library(detector_digi_hit_factory STATIC
    DetectorDigiHit_factory.cc
)

target_include_directories(detector_digi_hit_factory
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/data_objects
)

target_link_libraries(detector_digi_hit_factory
    PUBLIC
        fadc_parser
        services
)

set(DETECTOR_DIGI_HIT_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/data_objects
    PARENT_SCOPE
)

set(DETECTOR_DIGI_HIT_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/data_objects/HMSHodoscopeDigiHit.h
    PARENT_SCOPE
)

if(BUILD_TESTING)
    add_executable(detector_digi_hit_tests
        DetectorDigiHitTests.cc
    )
    target_link_libraries(detector_digi_hit_tests
        PRIVATE detector_digi_hit_factory
    )
    add_test(NAME detector_digi_hit_tests COMMAND detector_digi_hit_tests)
endif()
