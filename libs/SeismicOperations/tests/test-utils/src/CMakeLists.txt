set(OPERATIONS-TESTFILES

        # DATA GENERATORS
        ${CMAKE_CURRENT_SOURCE_DIR}/dummy-data-generators/DummyGridBoxGenerator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/dummy-data-generators/DummyConfigurationMapGenerator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/dummy-data-generators/DummyParametersGenerator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/dummy-data-generators/DummyModelGenerator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/dummy-data-generators/DummyTraceGenerator.cpp

        # NORM CALCULATOR
        ${CMAKE_CURRENT_SOURCE_DIR}/NumberHelpers.cpp

        # ENVIRONMENT HANDLERS
        ${CMAKE_CURRENT_SOURCE_DIR}/EnvironmentHandler.cpp

        ${OPERATIONS-TESTFILES}
        PARENT_SCOPE
        )