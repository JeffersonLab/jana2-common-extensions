add_jana_plugin(detector_translation
    SOURCES
        InitPlugin.cc
)

target_link_libraries(detector_translation
    PRIVATE
        ${JANA_LIBRARY}
        detector_mapping_api
        detector_digi_hit_processor
)
