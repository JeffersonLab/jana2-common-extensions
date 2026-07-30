cmake_minimum_required(VERSION 3.16)
project(jana2_common_extensions)

include(CTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Override with -DINSTALL_NAMESPACE=my_name at cmake configure time if needed
set(INSTALL_NAMESPACE "" CACHE STRING
    "Subdirectory namespace used in all install paths (include/, lib/cmake/, config/, etc.)" FORCE)

# Find dependencies
find_package(JANA REQUIRED)
find_package(evio REQUIRED)
find_package(ROOT REQUIRED COMPONENTS Core RIO Tree Hist)

# Add subdirectories
add_subdirectory(config)
add_subdirectory(scripts)
add_subdirectory(templates)
#add_subdirectory(src/libraries)
add_subdirectory(src/plugins)

# ── Unified package export ─────────────────────────────────────────────────────
# All install(TARGETS ... EXPORT jana2_common_extensionsTargets) calls across
# subdirectories accumulate into this single export set.  One find_package() call
# then gives consumers access to every exported target under the same namespace:
#   find_package(jana2_common_extensions REQUIRED)
#   target_link_libraries(my_plugin PRIVATE jana2_common_extensions::evio_parser_data_types)

include(CMakePackageConfigHelpers)

install(EXPORT jana2_common_extensionsTargets
    FILE jana2_common_extensionsConfig.cmake
    NAMESPACE jana2_common_extensions::
    DESTINATION lib/cmake/jana2_common_extensions
)
