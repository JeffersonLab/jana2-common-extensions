#
# CMake configuration for evio_parser plugin
#

# Build the setup-neutral EVIO libraries.
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

# add_jana_plugin sets INSTALL_RPATH explicitly, so append EVIO's lib64
# installation directory after the macro creates the target.
set_property(TARGET evio_parser APPEND PROPERTY
    INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib64"
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

target_include_directories(evio_parser_api INTERFACE
    $<INSTALL_INTERFACE:include/${INSTALL_NAMESPACE}/plugins/evio_parser>
)
target_link_libraries(evio_parser_api INTERFACE
    ${JANA_LIBRARY}
    evio::eviocc
    jce_config
)

add_library(evio_parser_data_types INTERFACE)
target_link_libraries(evio_parser_data_types INTERFACE evio_parser_api)

# Register into the project-wide export set; the install(EXPORT ...) call that
# writes the combined targets file lives in the root CMakeLists.txt.
install(TARGETS evio_parser_api evio_parser_data_types
    EXPORT jana2_common_extensionsTargets
)
