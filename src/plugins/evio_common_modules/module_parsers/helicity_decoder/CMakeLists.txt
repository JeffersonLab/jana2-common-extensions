#
# CMake configuration for helicity decoder module parsers
#

# Library containing helicity decoder module parser implementation
add_library(helicity_decoder_parsers STATIC
    ModuleParser_HelicityDecoder.cc
)

target_include_directories(helicity_decoder_parsers
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}       # helicity decoder parser headers
        ${CMAKE_CURRENT_SOURCE_DIR}/data_objects  # helicity decoder data object headers
)

# Link against core parser to get classes from which parser classes inherit
target_link_libraries(helicity_decoder_parsers
    PUBLIC
        core
)

set(HELDEC_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data_objects PARENT_SCOPE)

file(GLOB HELDEC_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/data_objects/*.h
)

set(HELDEC_PUBLIC_HEADERS ${HELDEC_PUBLIC_HEADERS} PARENT_SCOPE)


