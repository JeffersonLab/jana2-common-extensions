#
# CMake configuration for helicity scaler module parsers
#

# Library containing helicity scaler module parser implementation
add_library(helicity_scaler_parser STATIC
    ModuleParser_HelicityScaler.cc
)

target_include_directories(helicity_scaler_parser
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/data_objects
)

# Link against core parser library
target_link_libraries(helicity_scaler_parser
    PUBLIC
       core
)

set(HELICITY_SCALER_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data_objects PARENT_SCOPE)

file(GLOB HELICITY_SCALER_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/data_objects/*.h
)

set(HELICITY_SCALER_PUBLIC_HEADERS ${HELICITY_SCALER_PUBLIC_HEADERS} PARENT_SCOPE)
