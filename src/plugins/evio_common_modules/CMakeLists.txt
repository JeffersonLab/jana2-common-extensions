add_subdirectory(module_parsers)

add_jana_plugin(evio_common_modules
    SOURCES
        InitPlugin.cc
        module_parsers/InitModuleParsers.cc
    PUBLIC_HEADER
        ${MODULE_PARSERS_HEADERS}
)

target_link_libraries(evio_common_modules
    PRIVATE
        ${JANA_LIBRARY}
        evio_parser_api
        ${MODULE_PARSERS_LIBS}
)

add_library(evio_common_modules_data_types INTERFACE)
target_link_libraries(evio_common_modules_data_types INTERFACE evio_parser_api)
foreach(_dir IN LISTS MODULE_PARSERS_INCLUDE_DIRS)
    target_include_directories(evio_common_modules_data_types INTERFACE
        $<BUILD_INTERFACE:${_dir}>
    )
endforeach()
target_include_directories(evio_common_modules_data_types INTERFACE
    $<INSTALL_INTERFACE:include/${INSTALL_NAMESPACE}/plugins/evio_common_modules>
)
install(TARGETS evio_common_modules_data_types
    EXPORT jana2_common_extensionsTargets
)
