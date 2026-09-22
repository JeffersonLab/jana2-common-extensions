#
# CMake configuration for config
#

# Install all config files so they are available after make install
file(GLOB CONFIGS ${CMAKE_CURRENT_SOURCE_DIR}/*.db)

install(FILES ${CONFIGS}
    DESTINATION config/${INSTALL_NAMESPACE}
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                GROUP_READ GROUP_EXECUTE
                WORLD_READ WORLD_EXECUTE
)

install(
    DIRECTORY evio_parser/detector_mappings/
    DESTINATION config/${INSTALL_NAMESPACE}/evio_parser/detector_mappings
    FILES_MATCHING PATTERN "*.map"
)
