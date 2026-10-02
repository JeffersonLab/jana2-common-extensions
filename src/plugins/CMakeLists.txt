add_subdirectory(evio_parser)
add_subdirectory(evio_common_modules)
add_subdirectory(detector_translation)
add_subdirectory(evio_processor)
add_subdirectory(detector_translation_dump)

# Compatibility umbrella for existing downstream consumers.
target_link_libraries(evio_parser_data_types INTERFACE
    evio_common_modules_data_types
    hms_detector_translation_data_types
)
