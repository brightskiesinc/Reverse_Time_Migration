add_definitions(-DSTBX_TEST_DATA_PATH="${CMAKE_CURRENT_SOURCE_DIR}/test-data")

set(STBX-TESTFILES test_main.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/test-utils/include)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test-utils/src)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/agents)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/generators)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/parsers)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/writers)


enable_testing()
add_executable(seismic-toolbox-tests ${STBX-TESTFILES})
target_link_libraries(seismic-toolbox-tests SEISMIC-TOOLBOX)