add_library(detector_digi_hit_factory STATIC
    DetectorDigiHit_factory.cc
)

target_include_directories(detector_digi_hit_factory
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(detector_digi_hit_factory
    PUBLIC
        ${MODULE_PARSERS_LIBS}
        ${DETECTOR_TRANSLATOR_LIBS}
        services
)
