if (USE_OpenMp)
    set(STBX-SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/concrete/computation-parameters/omp/computation_parameters_generator.cpp
            ${STBX-SOURCES}
            )
elseif (USE_DPC)
    set(STBX-SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/concrete/computation-parameters/oneapi/computation_parameters_generator.cpp
            ${STBX-SOURCES}
            )
elseif (USE_OMP_Offload)
    set(STBX-SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/concrete/computation-parameters/omp-offload/computation_parameters_generator.cpp
            ${STBX-SOURCES}
            )
endif ()


set(STBX-SOURCES

        ${CMAKE_CURRENT_SOURCE_DIR}/Generator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/primitive/CallbacksGenerator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/primitive/ComponentsGenerator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/primitive/ConfigurationsGenerator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/primitive/ComputationParametersGetter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/primitive/ComponentsGenerator.cpp

        ${STBX-SOURCES}
        PARENT_SCOPE
        )