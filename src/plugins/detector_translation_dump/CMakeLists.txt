add_jana_plugin(detector_translation_dump
    SOURCES
        InitPlugin.cc
        JEventProcessor_DetectorTranslationDump.cc
)

target_link_libraries(detector_translation_dump
    PRIVATE
        ${JANA_LIBRARY}
        evio_parser_data_types
)
