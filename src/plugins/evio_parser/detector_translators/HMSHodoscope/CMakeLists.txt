add_library(hms_hodoscope_identity STATIC
    HMSHodoscopeIdentity.cc
)
target_include_directories(hms_hodoscope_identity
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(hms_hodoscope_identity
    PUBLIC detector_mapping
)

add_subdirectory(FADC)
add_subdirectory(FADCScaler)

add_library(hms_hodoscope_translators_init STATIC
    InitHMSHodoscopeTranslators.cc
)
target_include_directories(hms_hodoscope_translators_init
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(hms_hodoscope_translators_init
    PUBLIC
        hms_hodoscope_fadc_translator
        hms_hodoscope_fadc_scaler_translator
)

set(HMS_TRANSLATOR_LIBS
    hms_hodoscope_translators_init
    PARENT_SCOPE
)

set(HMS_TRANSLATOR_INCLUDE_DIRS
    ${HMS_HODOSCOPE_FADC_TRANSLATOR_INCLUDE_DIRS}
    ${HMS_HODOSCOPE_FADC_SCALER_TRANSLATOR_INCLUDE_DIRS}
    PARENT_SCOPE
)

set(HMS_TRANSLATOR_HEADERS
    ${HMS_HODOSCOPE_FADC_TRANSLATOR_PUBLIC_HEADERS}
    ${HMS_HODOSCOPE_FADC_SCALER_TRANSLATOR_PUBLIC_HEADERS}
    PARENT_SCOPE
)
