set(SOURCES

        # PRIMITIVE
        ${CMAKE_CURRENT_SOURCE_DIR}/primitive/Reader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/primitive/Writer.cpp

        # READERS
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/readers/BinaryReader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/readers/CSVReader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/readers/SegyReader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/readers/SUReader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/readers/TextReader.cpp

        # WRITERS
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/writers/BinaryWriter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/writers/CSVWriter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/writers/ImageWriter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/writers/SegyWriter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/writers/SUWriter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/writers/TextWriter.cpp

        # HELPERS
        ${CMAKE_CURRENT_SOURCE_DIR}/helpers/InStreamHelper.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/helpers/OutStreamHelper.cpp

        ${SOURCES}
        PARENT_SCOPE
        )