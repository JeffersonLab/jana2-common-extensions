#
# CMake configuration for FADC scaler module parsers
#

# Library containing FADC scaler module parser implementation
add_library(fadc_scaler_parser STATIC
    ModuleParser_FADCScaler.cc
)

target_include_directories(fadc_scaler_parser
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/data_objects
)

# Link against core parser library
target_link_libraries(fadc_scaler_parser
    PUBLIC
       core
)

set(FADC_SCALER_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data_objects PARENT_SCOPE)

file(GLOB FADC_SCALER_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/data_objects/*.h
)

set(FADC_SCALER_PUBLIC_HEADERS ${FADC_SCALER_PUBLIC_HEADERS} PARENT_SCOPE)


