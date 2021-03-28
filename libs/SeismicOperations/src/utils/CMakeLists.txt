set(OPERATIONS-SOURCES

        ${CMAKE_CURRENT_SOURCE_DIR}/filters/noise_filtering.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/interpolation/Interpolator.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/io/location_comparator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/io/read_utils.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/io/write_utils.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/sampling/Sampler.cpp

        ${OPERATIONS-SOURCES}
        PARENT_SCOPE
        )

set(OPERATIONS-LIBS

        MEMORY-MANAGER

        ${OPERATIONS-LIBS}
        PARENT_SCOPE
        )

add_subdirectory(compressor)
