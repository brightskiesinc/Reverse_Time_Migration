add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/configurations)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/data-units)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/helpers)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/indexers)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lookups)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/properties)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/streams)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/utils)


add_library(THOTH STATIC ${SOURCES})
target_link_libraries(THOTH ${LIBS})