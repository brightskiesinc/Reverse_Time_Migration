add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/configurations)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/logger)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/memory)

add_library(BS-BASE STATIC ${SOURCES_BASE})
target_link_libraries(BS-BASE ${LIBS})