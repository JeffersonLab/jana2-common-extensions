add_library(detector_mapping STATIC
    TranslationTable.cc
)

target_include_directories(detector_mapping
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include/${INSTALL_NAMESPACE}/plugins/evio_parser>
)

install(TARGETS detector_mapping
    EXPORT jana2_common_extensionsTargets
    ARCHIVE DESTINATION lib/${INSTALL_NAMESPACE}
)
