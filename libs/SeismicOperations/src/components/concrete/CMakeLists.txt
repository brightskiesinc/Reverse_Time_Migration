# TECHNOLOGY
if (USE_OpenMp)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/omp)
elseif (USE_DPC)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/oneapi)
elseif (USE_OMP_Offload)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/omp-offload)
endif ()

set(OPERATIONS-SOURCES ${OPERATIONS-SOURCES} PARENT_SCOPE)
