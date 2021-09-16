set(SOURCES

        # READERS
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/readers/SegyReader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/readers/SUReader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/readers/JsonReader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/readers/SeismicReader.cpp

        # WRITERS
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/writers/BinaryWriter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/writers/CSVWriter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/writers/ImageWriter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/writers/SegyWriter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/writers/SUWriter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/writers/SeismicWriter.cpp

        # HELPERS
        ${CMAKE_CURRENT_SOURCE_DIR}/helpers/InStreamHelper.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/helpers/OutStreamHelper.cpp

        ${SOURCES}
        PARENT_SCOPE
        )