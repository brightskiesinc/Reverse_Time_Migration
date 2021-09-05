set(SOURCES_BASE

        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/FileLogger.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/ConsoleLogger.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/LoggerCollection.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/LoggerChannel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/LoggerSystem.cpp

        ${SOURCES_BASE}
        PARENT_SCOPE
        )
