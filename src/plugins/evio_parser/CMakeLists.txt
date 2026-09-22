#
# CMake configuration for evio_parser plugin
#

# Build sub-libraries first (sets CORE_INCLUDE_DIR, CORE_PUBLIC_HEADERS,
# SERVICES_INCLUDE_DIR, SERVICES_PUBLIC_HEADERS, MODULE_PARSERS_INCLUDE_DIRS,
# MODULE_PARSERS_HEADERS, MODULE_PARSERS_LIBS, DETECTOR_TRANSLATOR_INCLUDE_DIRS,
# DETECTOR_TRANSLATOR_HEADERS, DETECTOR_TRANSLATOR_LIBS, PROCESSORS_LIBS in this
# scope)
add_subdirectory(core/detector_mapping_objects)
add_subdirectory(services)
add_subdirectory(core)
add_subdirectory(module_parsers)
add_subdirectory(detector_translators)
add_subdirectory(processors)

# Build the JANA2 plugin (shared library loaded at runtime via -Pplugins=evio_parser)
add_jana_plugin(evio_parser
    SOURCES
        InitPlugin.cc
        JEventSource_EVIO.cc
    PUBLIC_HEADER
        ${CORE_PUBLIC_HEADERS}
        ${SERVICES_PUBLIC_HEADERS}
        ${MODULE_PARSERS_HEADERS}
        ${DETECTOR_TRANSLATOR_HEADERS}
)

target_link_libraries(evio_parser
    PRIVATE
        ${JANA_LIBRARY}
        evio::eviocc
        core                # EvioEventParser + core data objects
        services            # JEventService_FilterDB, BankToModuleMap, ModuleParsersMap
)

# ── Exportable INTERFACE target ────────────────────────────────────────────────
# Exposes all data-type headers so downstream plugins can simply do:
#   target_link_libraries(my_plugin PRIVATE evio_parser_data_types)
#
# Generator expressions handle the build/install path split automatically:
#   - BUILD_INTERFACE  → source-tree data_objects/ directories (used during cmake build)
#   - INSTALL_INTERFACE → flat installed include dir  (used after make install)
# No caller ever needs to know or hardcode any path.

add_library(evio_parser_api INTERFACE)

foreach(_dir IN LISTS CORE_INCLUDE_DIR SERVICES_INCLUDE_DIR)
    target_include_directories(evio_parser_api INTERFACE
        $<BUILD_INTERFACE:${_dir}>
    )
endforeach()

# add_jana_plugin installs all PUBLIC_HEADERs flat into this directory
target_include_directories(evio_parser_api INTERFACE
    $<INSTALL_INTERFACE:include/${INSTALL_NAMESPACE}/plugins/evio_parser>
)

target_link_libraries(evio_parser_api INTERFACE
    ${JANA_LIBRARY}
    evio::eviocc
)

# Compatibility target for existing consumers. New code should link the API
# target because it includes parser and registration interfaces as well as data.
add_library(evio_parser_data_types INTERFACE)
target_link_libraries(evio_parser_data_types INTERFACE evio_parser_api)
foreach(_dir IN LISTS MODULE_PARSERS_INCLUDE_DIRS DETECTOR_TRANSLATOR_INCLUDE_DIRS)
    target_include_directories(evio_parser_data_types INTERFACE
        $<BUILD_INTERFACE:${_dir}>
    )
endforeach()
target_include_directories(evio_parser_data_types INTERFACE
    $<INSTALL_INTERFACE:include/${INSTALL_NAMESPACE}/plugins/evio_parser>
)

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
