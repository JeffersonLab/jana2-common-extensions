add_subdirectory(detector_mapping)
add_subdirectory(services)

add_library(detector_mapping_api INTERFACE)
target_link_libraries(detector_mapping_api INTERFACE
    detector_mapping
    detector_translation_services
    ${JANA_LIBRARY}
)
install(TARGETS detector_mapping_api
    EXPORT jana2_common_extensionsTargets
)

add_subdirectory(examples/hms_detector_translation)

add_jana_plugin(detector_translation
    SOURCES
        InitPlugin.cc
        JEventProcessor_DetectorDigiHits.cc
    PUBLIC_HEADER
        ${DETECTOR_MAPPING_PUBLIC_HEADERS}
        ${DETECTOR_TRANSLATION_SERVICES_PUBLIC_HEADERS}
)

target_link_libraries(detector_translation
    PRIVATE
        ${JANA_LIBRARY}
        detector_mapping_api
)
