# Set minimum cmake version.
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
cmake_policy(SET CMP0048 NEW)

# Project settings.
project(SeismicOperations LANGUAGES CXX VERSION 2.1.1)

if (CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
endif ()

# Set C++ standard to C++11 to allow for the advanced features used in code.
## In time of writing this, there are problems with std 17 for nvcc.
if (USE_CUDA)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
else()
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()


# Adding the headers directories needed for all project.
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/prerequisites)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Adding directories needed for all project.
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

option(BUILD_EXAMPLES "Option to enable building example programs" OFF)
if (BUILD_EXAMPLES)
    message(STATUS "Building Seismic Operations Examples")
endif ()

option(BUILD_TESTS "Option to enable building tests" OFF)
if (BUILD_TESTS)
    message(STATUS "Building Seismic Operations Tests")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
    include(CTest)
    add_test(NAME SEISMIC-OPERATIONS-TESTS
            COMMAND seismic-operations-tests
            )
endif ()
