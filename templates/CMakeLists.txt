#
# Generates jce_config_paths.h from the .h.in template and exposes it via the
# jce_config INTERFACE library.  Any plugin needing config-file resolution just
# does:
#
#   target_link_libraries(my_plugin PRIVATE jce_config)
#
# and calls jce_config_path("my_plugin", "my_file.db") — no per-plugin
# configure_file needed.

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/jce_config_paths.h.in
    ${CMAKE_BINARY_DIR}/include/jce_config_paths.h
    @ONLY
)

add_library(jce_config INTERFACE)
target_include_directories(jce_config INTERFACE
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include/${INSTALL_NAMESPACE}>
)

install(TARGETS jce_config
    EXPORT jana2_common_extensionsTargets
)

# Install the generated header for external consumers
install(FILES ${CMAKE_BINARY_DIR}/include/jce_config_paths.h
    DESTINATION include/${INSTALL_NAMESPACE}
)

# Install the template itself so external packages can reuse the same pattern
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/jce_config_paths.h.in
    DESTINATION templates/${INSTALL_NAMESPACE}
)
