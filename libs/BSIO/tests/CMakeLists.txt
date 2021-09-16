add_definitions(-DIO_TESTS_RESULTS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/test-results/")
add_definitions(-DIO_TESTS_WORKLOADS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/test-files/")

set(IO_TESTFILES ${CMAKE_CURRENT_SOURCE_DIR}/test_main.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/test-utils/include)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test-utils)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/test-utils/include)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test-utils/src)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/data-units)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/streams)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/utils)

enable_testing()
add_executable(bs-io-tests ${IO_TESTFILES})
target_link_libraries(bs-io-tests BS-IO)
