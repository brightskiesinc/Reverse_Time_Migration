add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/data-units)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/indexers)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lookups)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/streams)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/utils)

add_library(BS-IO STATIC ${SOURCES})
target_link_libraries(BS-IO ${LIBS} BS-BASE)