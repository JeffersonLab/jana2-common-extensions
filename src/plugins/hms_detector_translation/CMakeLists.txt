add_jana_plugin(hms_detector_translation
    SOURCES
        InitPlugin.cc
)

target_link_libraries(hms_detector_translation
    PRIVATE
        ${JANA_LIBRARY}
        detector_mapping_api
        hms_hodoscope_translators_init
)
