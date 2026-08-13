#
# CMake configuration for FADC module parsers
#

# Library containing FADC250 module parser implementation
add_library(fadc_parser STATIC
    ModuleParser_FADC.cc
)

target_include_directories(fadc_parser
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}       # FADC250 parser headers
        ${CMAKE_CURRENT_SOURCE_DIR}/data_objects  # FADC250 data object headers
)

# Link against core parser to get classes from which parser classes inherit
target_link_libraries(fadc_parser
    PUBLIC
        core
)

set(FADC_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data_objects PARENT_SCOPE)

file(GLOB FADC_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/data_objects/*.h
)

set(FADC_PUBLIC_HEADERS ${FADC_PUBLIC_HEADERS} PARENT_SCOPE)

if(BUILD_TESTING)
    add_executable(fadc250_daq_address_tests
        FADC250DAQAddressTests.cc
    )
    target_link_libraries(fadc250_daq_address_tests PRIVATE fadc_parser)
    add_test(
        NAME fadc250_daq_address_tests
        COMMAND fadc250_daq_address_tests
    )
endif()

