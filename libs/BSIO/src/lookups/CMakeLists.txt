set(SOURCES

        # MAPPERS
        ${CMAKE_CURRENT_SOURCE_DIR}/mappers/PSGHeaderMapper.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/mappers/SegyHeaderMapper.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/mappers/HeaderMapper.cpp

        # UTILS
        ${CMAKE_CURRENT_SOURCE_DIR}/SeismicFilesHeaders.cpp

        ${SOURCES}
        PARENT_SCOPE
        )