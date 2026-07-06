#
# CMake configuration for CAEN1190 module parsers
#

# Library containing CAEN1190 module parser implementation
add_library(caen1190_parser STATIC
    ModuleParser_CAEN1190.cc
)

target_include_directories(caen1190_parser
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}       # CAEN1190 parser headers
        ${CMAKE_CURRENT_SOURCE_DIR}/data_objects  # CAEN1190 data object headers
)

# Link against core parser to get classes from which parser classes inherit
target_link_libraries(caen1190_parser
    PUBLIC
        core
)

set(CAEN1190_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data_objects PARENT_SCOPE)

file(GLOB CAEN1190_PUBLIC_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/data_objects/*.h)

set(CAEN1190_PUBLIC_HEADERS ${CAEN1190_PUBLIC_HEADERS} PARENT_SCOPE)
