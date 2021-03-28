# Set minimum CMake version
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

# C++ standard and compiler options
find_program(BASH_EXECUTABLE NAMES bash REQUIRED)

# Intel Parallel Studio tools should be Sourced
# when using the cmake command or with the build script
if ("${USE_INTEL}" STREQUAL "")
    SET(USE_INTEL "NO")
endif ()

option(USE_OpenMp "Use OpenMp technology" OFF)
option(USE_DPC "Use DPC++ technology" OFF)
message(STATUS "Use OpenMp ${USE_OpenMp} and Use DPC++ ${USE_DPC} and Use Cuda ${USE_CUDA}")

if (NOT USE_OpenMp AND NOT USE_DPC)
    message(FATAL_ERROR "Please specify a technology using the config.sh script")
elseif (USE_OpenMp AND USE_DPC)
    message(FATAL_ERROR "Please specify only one technology using the config.sh script")
elseif (USE_OpenMp)
    if (${USE_INTEL} STREQUAL "YES")
        set(CMAKE_C_COMPILER icc)
        set(CMAKE_CXX_COMPILER icpc)

        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}  -xHost -qopenmp -g -debug -O3 -fp-model fast=2 -no-prec-div -fma -qopt-assume-safe-padding -qopt-report=4")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}  -xHost -qopenmp -O3 -qopt-report=4 -fp-model fast=2 -no-prec-div -fma -qopt-assume-safe-padding")
    else ()
        set(CMAKE_C_COMPILER gcc)
        set(CMAKE_CXX_COMPILER g++)

        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fopenmp -g -ftree-vectorize -O3 -fopt-info-vec-optimized ")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fopenmp -ftree-vectorize -O3 -fopt-info-vec-optimized ")
    endif ()
elseif (USE_DPC)
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER dpcpp)
    set(CMAKE_CXX_FLAGS_DEBUG "-fsycl -O3 --std=c++17")
    set(CMAKE_CXX_FLAGS_RELEASE "-fsycl -O3 --std=c++17")
    set(CMAKE_CXX_FLAGS "-fsycl -O3 --std=c++17")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lOpenCL -lsycl")
endif ()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "" OR ${CMAKE_BUILD_TYPE} STREQUAL "NOMODE")
    message(STATUS "WORKING ON NO MODE")
elseif (${CMAKE_BUILD_TYPE} STREQUAL "RELEASE")
    message(STATUS "WORKING ON RELEASE MODE")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_RELEASE}")
elseif (${CMAKE_BUILD_TYPE} STREQUAL "DEBUG" OR ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    message(STATUS "WORKING ON DEBUG MODE")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG}")
else ()
    message(FATAL_ERROR "Unrecognized build type")
endif ()

message(STATUS "Flags : ${CMAKE_CXX_FLAGS}")

##############################################################################
# COMPILER DEFINITIONS
###############################################################################
if ("${DATA_PATH}" STREQUAL "")
    SET(DATA_PATH "./data")
endif ()
if ("${WRITE_PATH}" STREQUAL "")
    SET(WRITE_PATH "./results")
endif ()
if ("${WORKLOAD_PATH}" STREQUAL "")
    SET(WORKLOAD_PATH "./workloads/bp_model")
endif ()
if ("${COMPRESSION}" STREQUAL "")
    SET(COMPRESSION "NO")
endif ()

add_definitions(
        -DDATA_PATH="${DATA_PATH}"
        -DWRITE_PATH="${WRITE_PATH}"
        -DWORKLOAD_PATH="${WORKLOAD_PATH}")

##############################################################################
# PROJECT SETTINGS
##############################################################################

# Project name and language
project(Seismic-Toolbox LANGUAGES CXX VERSION 3.0.0)


##############################################################################
# LIBRARIES SETTINGS
##############################################################################

option(USE_MPI "Use MPI technology" OFF)
if (USE_MPI)
    add_definitions(-DUSING_MPI)

    message(STATUS "Using MPI shot distribution")
    find_package(MPI REQUIRED)

    include_directories(${MPI_INCLUDE_PATH})
endif ()

option(USE_OpenCV "Use OpenCV technology" OFF)
if (USE_OpenCV)
    message(STATUS "Using OpenCV technology")
    find_package(OpenCV REQUIRED)
    include_directories(${OpenCV_INCLUDE_DIRS})
else ()
    message(STATUS "No OpenCV used")
endif ()

##############################################################################
# HEADERS INCLUDE
##############################################################################

# Adding the headers directories needed for all projects.
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/prerequisites)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/Helpers/MemoryManager/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/Helpers/Timer/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/seismic-io-framework)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/seismic-io-framework/Segy)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/seismic-io-framework/visualization)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/SeismicOperations/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/Thoth/include)
##############################################################################
# SUBDIRECTORIES ADDITION
##############################################################################

# TECHNOLOGY
if (USE_OpenMp)
    add_definitions(-DUSING_OMP)
elseif (USE_DPC)
    add_definitions(-DUSING_DPCPP)
endif ()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/Helpers)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/seismic-io-framework)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/SeismicOperations)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/Thoth)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

##############################################################################
# EXECUTABLES
##############################################################################

add_executable(Engine main_migration.cpp)
target_link_libraries(Engine SEISMIC-OPERATIONS SEISMIC-TOOLBOX)

add_executable(Modeller main_modelling.cpp)
target_link_libraries(Modeller SEISMIC-OPERATIONS SEISMIC-TOOLBOX)

##############################################################################
# EXAMPLES
##############################################################################

option(BUILD_EXAMPLES "Option to enable building example programs" OFF)
if (BUILD_EXAMPLES)
    message(STATUS "Building Seismic Toolbox Examples")
endif ()

##############################################################################
# TESTING
##############################################################################

option(BUILD_TESTS "Option to enable building tests" OFF)
if (BUILD_TESTS)
    message(STATUS "Building Seismic Toolbox Tests")
    add_subdirectory(tests)
    include(CTest)
    add_test(NAME SEISMIC-TOOLBOX-TESTS
            COMMAND seismic-toolbox-tests
            )
endif ()
