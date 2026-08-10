add_subdirectory(detector_digi_hits)

set(FACTORIES_LIBS
    detector_digi_hit_factory
    PARENT_SCOPE
)

set(FACTORIES_INCLUDE_DIRS
    ${DETECTOR_DIGI_HIT_INCLUDE_DIRS}
    PARENT_SCOPE
)

set(FACTORIES_HEADERS
    ${DETECTOR_DIGI_HIT_PUBLIC_HEADERS}
    PARENT_SCOPE
)
