#
# CMake configuration for core parser utilities
#

add_library(core STATIC
    EvioEventParser.cc
)

target_link_libraries(core
    PUBLIC
        services          # Service utilities
        evio::eviocc      # EVIO C++ library for data I/O
        ${JANA_LIBRARY}   # JANA2 framework library
)

target_include_directories(core
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/data_objects
)

set(CORE_INCLUDE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/data_objects
    PARENT_SCOPE
)

file(GLOB CORE_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/data_objects/*.h
)

set(CORE_PUBLIC_HEADERS ${CORE_PUBLIC_HEADERS} PARENT_SCOPE)
