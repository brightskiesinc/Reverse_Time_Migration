# TECHNOLOGY
if (USE_OpenMp)
    add_subdirectory(omp)
elseif (USE_DPC)
    add_subdirectory(oneapi)
endif ()

set(OPERATIONS-SOURCES ${OPERATIONS-SOURCES} PARENT_SCOPE)
