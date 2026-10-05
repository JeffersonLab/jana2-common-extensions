cmake_minimum_required(VERSION 3.22)
project(jce_superbuild NONE)

include(ExternalProject)
find_package(Git REQUIRED)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    message(FATAL_ERROR
        "Set CMAKE_INSTALL_PREFIX to the directory where the JCE stack should be installed")
endif()

set(JCE_JANA_GIT_REPOSITORY
    "https://github.com/JeffersonLab/JANA2.git"
    CACHE STRING "JANA2 Git repository")
set(JCE_JANA_GIT_TAG
    "v2026.03.01"
    CACHE STRING "Pinned JANA2 Git tag or commit")
set(JCE_EVIO_GIT_REPOSITORY
    "https://github.com/JeffersonLab/evio.git"
    CACHE STRING "EVIO Git repository")
set(JCE_EVIO_GIT_TAG
    "v6.1.2"
    CACHE STRING "Pinned EVIO Git tag or commit")
set(JANA_SOURCE_DIR ""
    CACHE PATH "Existing JANA2 source directory (avoids downloading JANA2)")
set(EVIO_SOURCE_DIR ""
    CACHE PATH "Existing EVIO source directory (avoids downloading EVIO)")
option(JCE_SUPERBUILD_EVIO_PROCESSOR
    "Build the ROOT-based JCE evio_processor plugin"
    OFF)

get_filename_component(JCE_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

set(_jce_prefix_path "${CMAKE_INSTALL_PREFIX}")
if(CMAKE_PREFIX_PATH)
    list(APPEND _jce_prefix_path ${CMAKE_PREFIX_PATH})
endif()
list(JOIN _jce_prefix_path "|" _jce_prefix_path_arg)

if(JANA_SOURCE_DIR)
    get_filename_component(JANA_SOURCE_DIR "${JANA_SOURCE_DIR}" ABSOLUTE)
    if(NOT EXISTS "${JANA_SOURCE_DIR}/CMakeLists.txt")
        message(FATAL_ERROR "JANA_SOURCE_DIR does not contain CMakeLists.txt: ${JANA_SOURCE_DIR}")
    endif()
    ExternalProject_Add(jana
        SOURCE_DIR "${JANA_SOURCE_DIR}"
        DOWNLOAD_COMMAND ""
        UPDATE_COMMAND ""
        CMAKE_CACHE_ARGS
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
            -DCMAKE_CXX_STANDARD:STRING=20
            -DBUILD_EXAMPLES:BOOL=OFF
            -DBUILD_TESTS:BOOL=OFF
            -DUSE_ROOT:BOOL=OFF
    )
else()
    ExternalProject_Add(jana
        GIT_REPOSITORY "${JCE_JANA_GIT_REPOSITORY}"
        GIT_TAG "${JCE_JANA_GIT_TAG}"
        GIT_SHALLOW TRUE
        UPDATE_DISCONNECTED TRUE
        CMAKE_CACHE_ARGS
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
            -DCMAKE_CXX_STANDARD:STRING=20
            -DBUILD_EXAMPLES:BOOL=OFF
            -DBUILD_TESTS:BOOL=OFF
            -DUSE_ROOT:BOOL=OFF
    )
endif()

if(EVIO_SOURCE_DIR)
    get_filename_component(EVIO_SOURCE_DIR "${EVIO_SOURCE_DIR}" ABSOLUTE)
    if(NOT EXISTS "${EVIO_SOURCE_DIR}/CMakeLists.txt")
        message(FATAL_ERROR "EVIO_SOURCE_DIR does not contain CMakeLists.txt: ${EVIO_SOURCE_DIR}")
    endif()
    ExternalProject_Add(evio
        DOWNLOAD_COMMAND
            "${CMAKE_COMMAND}" -E copy_directory "${EVIO_SOURCE_DIR}" <SOURCE_DIR>
        UPDATE_COMMAND ""
        PATCH_COMMAND
            "${GIT_EXECUTABLE}" apply --whitespace=nowarn
            "${CMAKE_CURRENT_LIST_DIR}/patches/evio-respect-install-prefix.patch"
        CMAKE_CACHE_ARGS
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
            -DMAKE_EXAMPLES:BOOL=OFF
    )
else()
    ExternalProject_Add(evio
        GIT_REPOSITORY "${JCE_EVIO_GIT_REPOSITORY}"
        GIT_TAG "${JCE_EVIO_GIT_TAG}"
        GIT_SHALLOW TRUE
        UPDATE_DISCONNECTED TRUE
        PATCH_COMMAND
            "${GIT_EXECUTABLE}" apply --whitespace=nowarn
            "${CMAKE_CURRENT_LIST_DIR}/patches/evio-respect-install-prefix.patch"
        CMAKE_CACHE_ARGS
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
            -DMAKE_EXAMPLES:BOOL=OFF
    )
endif()

ExternalProject_Add(jce
    SOURCE_DIR "${JCE_SOURCE_DIR}"
    DOWNLOAD_COMMAND ""
    UPDATE_COMMAND ""
    DEPENDS jana evio
    LIST_SEPARATOR "|"
    CMAKE_CACHE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_PREFIX_PATH:PATH=${_jce_prefix_path_arg}
        -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
        -DBUILD_TESTING:BOOL=OFF
        -DJCE_BUILD_EVIO_PROCESSOR:BOOL=${JCE_SUPERBUILD_EVIO_PROCESSOR}
    BUILD_ALWAYS TRUE
)
