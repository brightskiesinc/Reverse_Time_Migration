set(OPERATIONS-TESTFILES

        # MODEL HANDLERS
        ${CMAKE_CURRENT_SOURCE_DIR}/model-handlers/TestSeismicModelHandler.cpp

        # MIGRATION ACCOMODATORS
        ${CMAKE_CURRENT_SOURCE_DIR}/migration-accommodators/TestCrossCorrelationKernel.cpp

        # TRACE MANAGERS
        ${CMAKE_CURRENT_SOURCE_DIR}/trace-managers/TestSeismicTraceManager.cpp

        # SOURCE INJECTORS
        ${CMAKE_CURRENT_SOURCE_DIR}/source-injectors/TestRickerSourceInjector.cpp

        # COMPUTATION KERNELS
        ${CMAKE_CURRENT_SOURCE_DIR}/computation-kernels/iso/TestSecondOrderComputationKernel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/computation-kernels/iso/TestStaggeredComputationKernel.cpp

        # FORWARD COLLECTORS
        ${CMAKE_CURRENT_SOURCE_DIR}/forward-collectors/TestReversePropagation.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/forward-collectors/TestTwoPropagation.cpp

        ${OPERATIONS-TESTFILES}
        PARENT_SCOPE
        )
