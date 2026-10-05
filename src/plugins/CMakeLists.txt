add_subdirectory(evio_parser)
add_subdirectory(evio_common_modules)
add_subdirectory(detector_translation)

if(JCE_BUILD_EVIO_PROCESSOR)
    add_subdirectory(evio_processor)
endif()

add_subdirectory(detector_translation_dump)

# Compatibility umbrella for existing downstream consumers.
target_link_libraries(evio_parser_data_types INTERFACE
    evio_common_modules_data_types
    hms_detector_translation_data_types
)
