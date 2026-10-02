#
# CMake configuration for MPD module parsers
#

# Library containing MPD module parser implementation
add_library(mpd_parser STATIC
    ModuleParser_MPD.cc
)

target_include_directories(mpd_parser
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}       # MPD parser headers
        ${CMAKE_CURRENT_SOURCE_DIR}/data_objects  # MPD data object headers
)

# Link against core parser to get classes from which parser classes inherit
target_link_libraries(mpd_parser
    PUBLIC
        core
)

set(MPD_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data_objects PARENT_SCOPE)

file(GLOB MPD_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/data_objects/*.h
)

set(MPD_PUBLIC_HEADERS ${MPD_PUBLIC_HEADERS} PARENT_SCOPE)


