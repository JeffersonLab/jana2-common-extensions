add_subdirectory(
    ../evio_parser/detector_translators
    ${CMAKE_CURRENT_BINARY_DIR}/detector_translators
)

add_jana_plugin(hms_detector_translation
    SOURCES
        InitPlugin.cc
    PUBLIC_HEADER
        ${DETECTOR_TRANSLATOR_HEADERS}
)

target_link_libraries(hms_detector_translation
    PRIVATE
        ${JANA_LIBRARY}
        detector_mapping_api
        hms_hodoscope_translators_init
)

add_library(hms_detector_translation_data_types INTERFACE)
target_link_libraries(hms_detector_translation_data_types INTERFACE
    evio_common_modules_data_types
    detector_mapping_api
)
foreach(_dir IN LISTS DETECTOR_TRANSLATOR_INCLUDE_DIRS)
    target_include_directories(hms_detector_translation_data_types INTERFACE
        $<BUILD_INTERFACE:${_dir}>
    )
endforeach()
target_include_directories(hms_detector_translation_data_types INTERFACE
    $<INSTALL_INTERFACE:include/${INSTALL_NAMESPACE}/plugins/hms_detector_translation>
)
install(TARGETS hms_detector_translation_data_types
    EXPORT jana2_common_extensionsTargets
)
