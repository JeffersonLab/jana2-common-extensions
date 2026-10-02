add_subdirectory(detector_mapping)
add_subdirectory(services)

add_library(detector_mapping_api INTERFACE)
target_include_directories(detector_mapping_api INTERFACE
    $<BUILD_INTERFACE:${DETECTOR_MAPPING_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${DETECTOR_TRANSLATION_SERVICES_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include/${INSTALL_NAMESPACE}/plugins/detector_translation>
)
target_link_libraries(detector_mapping_api INTERFACE
    detector_mapping
    detector_translation_services
    ${JANA_LIBRARY}
)
install(TARGETS detector_mapping_api
    EXPORT jana2_common_extensionsTargets
)

add_subdirectory(processors)
add_subdirectory(examples/hms_detector_translation)

add_jana_plugin(detector_translation
    SOURCES
        InitPlugin.cc
    PUBLIC_HEADER
        ${DETECTOR_MAPPING_PUBLIC_HEADERS}
        ${DETECTOR_TRANSLATION_SERVICES_PUBLIC_HEADERS}
)

target_link_libraries(detector_translation
    PRIVATE
        ${JANA_LIBRARY}
        detector_mapping_api
        detector_digi_hit_processor
)
