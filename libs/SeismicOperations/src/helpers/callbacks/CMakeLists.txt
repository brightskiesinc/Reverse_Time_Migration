set(OPERATIONS-SOURCES

        ${CMAKE_CURRENT_SOURCE_DIR}/primitive/CallbackCollection.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/interface/WriterCallback.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/BinaryWriter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/CSVWriter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/ImageWriter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/NormWriter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/SegyWriter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/SUWriter.cpp

        ${OPERATIONS-SOURCES}
        PARENT_SCOPE
        )