add_library(detector_mapping STATIC
    TranslationTable.cc
)

target_include_directories(detector_mapping
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
