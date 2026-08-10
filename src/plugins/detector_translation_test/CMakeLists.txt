add_jana_plugin(detector_translation_test
    SOURCES
        InitPlugin.cc
        JEventProcessor_DetectorTranslationTest.cc
)

target_link_libraries(detector_translation_test
    PRIVATE
        ${JANA_LIBRARY}
        evio_parser_data_types
)
