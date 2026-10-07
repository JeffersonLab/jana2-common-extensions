#
# CMake configuration for helper scripts (e.g. Valgrind checks)
#

set(VALGRIND_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/valgrind_check.csh")

add_custom_target(valgrindCheck
    COMMAND ${CMAKE_COMMAND} -E echo "Running Valgrind memory check..."
    # Run the script from the project root so relative paths (e.g. build/) match
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..
    COMMAND ${VALGRIND_SCRIPT}
    VERBATIM
)

# Install all scripts so they are available after make install
file(GLOB SCRIPTS ${CMAKE_CURRENT_SOURCE_DIR}/*.csh
                  ${CMAKE_CURRENT_SOURCE_DIR}/*.sh
                  ${CMAKE_CURRENT_SOURCE_DIR}/*.py)

install(FILES ${SCRIPTS}
    DESTINATION scripts/${INSTALL_NAMESPACE}
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                GROUP_READ GROUP_EXECUTE
                WORLD_READ WORLD_EXECUTE
)

if(BUILD_TESTING)
    find_package(Python3 QUIET COMPONENTS Interpreter)
    if(Python3_Interpreter_FOUND)
        add_test(NAME plugin_layers_tests
            COMMAND ${Python3_EXECUTABLE}
                ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_plugin_layers.py)
    endif()
endif()
