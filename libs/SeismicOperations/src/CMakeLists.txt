add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/backend)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/components)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/configurations)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/data-units)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/engines)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/helpers)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/utils)

set(OPERATIONS-LIBS

        FILE-COMPRESSION
        TIMER
        MEMORY-MANAGER
        SEGY-TOOLS
        THOTH
        SEISMIC-IO
        openCV-vis

        ${OPERATIONS-LIBS}
        )

add_library(SEISMIC-OPERATIONS STATIC ${OPERATIONS-SOURCES})
target_link_libraries(SEISMIC-OPERATIONS ${OPERATIONS-LIBS})

