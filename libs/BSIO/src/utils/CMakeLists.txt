set(SOURCES

        ${CMAKE_CURRENT_SOURCE_DIR}/displayers/Displayers.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/checkers/Checker.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/convertors/NumbersConvertor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/convertors/StringsConvertor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/convertors/FloatingPointFormatter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/convertors/KeysConvertor.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/synthetic-generators/concrete/ParameterMetaDataGenerator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/synthetic-generators/concrete/ShotsMetaDataGenerator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/synthetic-generators/concrete/PlaneReflectorGenerator.cpp

        ${SOURCES}
        PARENT_SCOPE
        )