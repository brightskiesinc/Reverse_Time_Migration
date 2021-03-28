set(OPERATIONS-SOURCES

        # COMPUTATION KERNELS
        ${CMAKE_CURRENT_SOURCE_DIR}/computation-kernels/isotropic/SecondOrderComputationKernel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/computation-kernels/isotropic/StaggeredComputationKernel.cpp

        # MIGRATION ACCOMMODATORS
        ${CMAKE_CURRENT_SOURCE_DIR}/migration-accommodators/CrossCorrelationKernel.cpp

        # BOUNDARIES COMPONENTS
        ${CMAKE_CURRENT_SOURCE_DIR}/boundary-managers/extensions/ZeroExtension.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/boundary-managers/extensions/RandomExtension.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/boundary-managers/extensions/HomogenousExtension.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/boundary-managers/extensions/MinExtension.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/boundary-managers/SpongeBoundaryManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/boundary-managers/CPMLBoundaryManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/boundary-managers/StaggeredCPMLBoundaryManager.cpp

        # TRACE MANAGERS
        ${CMAKE_CURRENT_SOURCE_DIR}/trace-managers/SeismicTraceManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/trace-managers/BinaryTraceManager.cpp

        # SOURCE INJECTORS
        ${CMAKE_CURRENT_SOURCE_DIR}/source-injectors/RickerSourceInjector.cpp

        # MEMORY HANDLERS
        ${CMAKE_CURRENT_SOURCE_DIR}/memory-handlers/WaveFieldsMemoryHandler.cpp

        # MODEL HANDLERS
        ${CMAKE_CURRENT_SOURCE_DIR}/model-handlers/SyntheticModelHandler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/model-handlers/SeismicModelHandler.cpp

        # FRAME BUFFER
        ${CMAKE_CURRENT_SOURCE_DIR}/data-units/FrameBuffer.cpp

        # TRACE WRITERS
        ${CMAKE_CURRENT_SOURCE_DIR}/trace-writers/BinaryTraceWriter.cpp

        ${OPERATIONS-SOURCES}
        PARENT_SCOPE
        )