#
# CMake configuration for services
#

add_library(services STATIC
    JEventService_FilterDB.cc
)

target_include_directories(services
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../core>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../core/data_objects>
        $<INSTALL_INTERFACE:include/${INSTALL_NAMESPACE}/plugins/evio_parser>
)

install(TARGETS services
    EXPORT jana2_common_extensionsTargets
    ARCHIVE DESTINATION lib/${INSTALL_NAMESPACE}
)

target_link_libraries(services
    PUBLIC
        ${JANA_LIBRARY}                # JANA2 framework library
        evio::eviocc                   # ModuleParser.h includes eviocc.h
        jce_config                     # Provides jce_config_paths.h to services and their consumers
)

set(SERVICES_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

file(GLOB SERVICES_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

set(SERVICES_PUBLIC_HEADERS ${SERVICES_PUBLIC_HEADERS} PARENT_SCOPE)

if(BUILD_TESTING)
    add_executable(config_layers_tests tests/ConfigLayersTests.cc)
    target_link_libraries(config_layers_tests PRIVATE services)
    add_test(NAME config_layers_tests COMMAND config_layers_tests)

    add_executable(bank_to_module_map_tests
        tests/JEventService_BankToModuleMapTests.cc
    )
    target_link_libraries(bank_to_module_map_tests PRIVATE services)
    add_test(
        NAME bank_to_module_map_tests
        COMMAND bank_to_module_map_tests
    )

    add_executable(module_parsers_map_tests
        tests/JEventService_ModuleParsersMapTests.cc
    )
    target_link_libraries(module_parsers_map_tests PRIVATE services)
    add_test(
        NAME module_parsers_map_tests
        COMMAND module_parsers_map_tests
    )

endif()
