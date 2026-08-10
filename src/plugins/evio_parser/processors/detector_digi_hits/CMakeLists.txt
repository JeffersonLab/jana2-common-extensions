add_library(detector_digi_hit_processor STATIC
    JEventProcessor_DetectorDigiHits.cc
)

target_include_directories(detector_digi_hit_processor
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(detector_digi_hit_processor
    PUBLIC
        ${DETECTOR_TRANSLATOR_LIBS}
        ${MODULE_PARSERS_LIBS}
        services
)
