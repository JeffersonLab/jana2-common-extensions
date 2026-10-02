#
# CMake configuration for TI scaler module parsers
#

# Library containing TI scaler module parser implementation
add_library(tiscaler_parser STATIC
    ModuleParser_TIScaler.cc
)

target_include_directories(tiscaler_parser
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/data_objects
)

# Link against core parser library
target_link_libraries(tiscaler_parser
    PUBLIC
        core
)

set(TISCALER_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data_objects PARENT_SCOPE)

file(GLOB TISCALER_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/data_objects/*.h
)

set(TISCALER_PUBLIC_HEADERS ${TISCALER_PUBLIC_HEADERS} PARENT_SCOPE)


