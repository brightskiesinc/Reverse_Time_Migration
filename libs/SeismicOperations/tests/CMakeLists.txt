add_definitions(-DOPERATIONS_TEST_DATA_PATH="${CMAKE_CURRENT_SOURCE_DIR}/test-data")

set(OPERATIONS-TESTFILES test_main.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/test-utils/include)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test-utils/src)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/common)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/components)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/data-units)

enable_testing()
add_executable(seismic-operations-tests ${OPERATIONS-TESTFILES})
target_link_libraries(seismic-operations-tests SEISMIC-OPERATIONS)
