#
# CMake configuration for FADC module parsers
#

# Library containing faV3compton module parser implementation
add_library(faV3compton_parser STATIC
    ModuleParser_faV3compton.cc
)

target_include_directories(faV3compton_parser
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}       # faV3compton parser headers
        ${CMAKE_CURRENT_SOURCE_DIR}/data_objects  # faV3compton data object headers
)

# Link against core parser to get classes from which parser classes inherit
target_link_libraries(faV3compton_parser
    PUBLIC
        core
)

set(FAV3COMPTON_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data_objects PARENT_SCOPE)

file(GLOB FAV3COMPTON_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/data_objects/*.h
)

set(FAV3COMPTON_PUBLIC_HEADERS ${FADC_PUBLIC_HEADERS} PARENT_SCOPE)


