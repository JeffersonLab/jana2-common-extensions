#
# CMake configuration for evio_parser plugin
#

# Build the setup-neutral EVIO and mapping libraries.
add_subdirectory(core/detector_mapping_objects)
add_subdirectory(services)
add_subdirectory(core)

# Build the JANA2 plugin (shared library loaded at runtime via -Pplugins=evio_parser)
add_jana_plugin(evio_parser
    SOURCES
        InitPlugin.cc
        JEventSource_EVIO.cc
    PUBLIC_HEADER
        ${CORE_PUBLIC_HEADERS}
        ${SERVICES_PUBLIC_HEADERS}
)

target_link_libraries(evio_parser
    PRIVATE
        ${JANA_LIBRARY}
        evio::eviocc
        core                # EvioEventParser + core data objects
        services            # JEventService_FilterDB, BankToModuleMap, ModuleParsersMap
)

add_library(evio_parser_api INTERFACE)

foreach(_dir IN LISTS CORE_INCLUDE_DIR SERVICES_INCLUDE_DIR)
    target_include_directories(evio_parser_api INTERFACE
        $<BUILD_INTERFACE:${_dir}>
    )
endforeach()

# add_jana_plugin installs the core and service headers here.
target_include_directories(evio_parser_api INTERFACE
    $<INSTALL_INTERFACE:include/${INSTALL_NAMESPACE}/plugins/evio_parser>
)

target_link_libraries(evio_parser_api INTERFACE
    ${JANA_LIBRARY}
    evio::eviocc
)

# Compatibility target extended with common and HMS types in src/plugins.
add_library(evio_parser_data_types INTERFACE)
target_link_libraries(evio_parser_data_types INTERFACE evio_parser_api)

# Register into the project-wide export set; the install(EXPORT ...) call that
# writes the combined targets file lives in the root CMakeLists.txt.
install(TARGETS evio_parser_api evio_parser_data_types
    EXPORT jana2_common_extensionsTargets
)

# Public API for setup plugins which provide detector-specific translators.
add_library(detector_mapping_api INTERFACE)
target_include_directories(detector_mapping_api INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/core/detector_mapping_objects>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/services>
    $<INSTALL_INTERFACE:include/${INSTALL_NAMESPACE}/plugins/evio_parser>
)
target_link_libraries(detector_mapping_api INTERFACE
    detector_mapping
    services
    ${JANA_LIBRARY}
)
install(TARGETS detector_mapping_api
    EXPORT jana2_common_extensionsTargets
)
