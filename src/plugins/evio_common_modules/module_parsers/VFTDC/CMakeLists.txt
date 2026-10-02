#
# CMake configuration for VFTDC module parsers
#

# Library containing VFTDC module parser implementation
add_library(vftdc_parser STATIC
    ModuleParser_VFTDC.cc
)

target_include_directories(vftdc_parser
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}       # VFTDC parser headers
        ${CMAKE_CURRENT_SOURCE_DIR}/data_objects  # VFTDC data object headers
)

# Link against core parser to get classes from which parser classes inherit
target_link_libraries(vftdc_parser
    PUBLIC
        core
)

set(VFTDC_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data_objects PARENT_SCOPE)

file(GLOB VFTDC_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/data_objects/*.h
)

set(VFTDC_PUBLIC_HEADERS ${VFTDC_PUBLIC_HEADERS} PARENT_SCOPE)
