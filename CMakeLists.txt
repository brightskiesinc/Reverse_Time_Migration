# Copyright (C) 2021 by Brightskies inc
#
# This file is part of SeismicToolbox.
#
# SeismicToolbox is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# SeismicToolbox is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.


# Set minimum CMake version
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
#cmake_policy(SET CMP0104 NEW)

# C++ standard and compiler options
find_program(BASH_EXECUTABLE NAMES bash REQUIRED)

# Intel Parallel Studio tools should be Sourced
# when using the cmake command or with the build script
if ("${USE_INTEL}" STREQUAL "")
    SET(USE_INTEL "NO")
endif ()

option(USE_OMP "Use OpenMP technology" OFF)
option(USE_DPC "Use DPC++ technology" OFF)
option(USE_OMP_OFFLOAD "Use OpenMP Offload technology" OFF)

message(STATUS "Use OpenMp ${USE_OMP}, Use DPC++ ${USE_DPC} and Use OpenMP Offload ${USE_OMP_OFFLOAD}")

if (NOT USE_OMP AND NOT USE_DPC AND NOT USE_OMP_OFFLOAD)
    message(FATAL_ERROR "Please specify a technology using the config.sh script")
elseif (USE_OMP AND USE_DPC OR USE_OMP AND USE_OMP_OFFLOAD OR USE_DPC AND USE_OMP_OFFLOAD)
    message(FATAL_ERROR "Please specify only one technology using the config.sh script")
elseif (${USE_OMP})
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
elseif (${USE_DPC})
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER dpcpp)
    set(CMAKE_CXX_FLAGS_DEBUG "-O3 -fsycl -std=c++17 -g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -fsycl -std=c++17")
    set(CMAKE_CXX_FLAGS " -O3 -fsycl -std=c++17")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lOpenCL -lsycl")
elseif (${USE_OMP_OFFLOAD})
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER icpx)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fiopenmp -std=c++17 -fopenmp-targets=spir64 -O3 -D__STRICT_ANSI__  ")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fiopenmp -std=c++17 -fopenmp-targets=spir64 -O3 -D__STRICT_ANSI__  ")
    set(CMAKE_CXX_FLAGS "-fiopenmp -std=c++17 -fopenmp-targets=spir64 -O3 -D__STRICT_ANSI__ ")
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
project(Seismic-Toolbox LANGUAGES CXX VERSION 2.2.1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

find_package(spdlog)
if (spdlog_FOUND)
    message(STATUS "spdlog library is found")
    add_definitions(-DUSING_SPDLOG)
endif ()

##############################################################################
# HEADERS INCLUDE
##############################################################################

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/BSBase)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/BSBase/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/BSTimer/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/BSIO/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/SeismicOperations/include)

# Adding the headers directories needed for all projects.
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/prerequisites)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

##############################################################################
# SUBDIRECTORIES ADDITION
##############################################################################

# TECHNOLOGY
if (USE_OMP)
    add_definitions(-DUSING_OMP)
elseif (USE_DPC)
    add_definitions(-DUSING_DPCPP)
elseif (USE_OMP_OFFLOAD)
    add_definitions(-DUSING_OMP_OFFLOAD)

endif ()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/BSBase)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/BSTimer)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/BSIO)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/SeismicOperations)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

##############################################################################
# EXECUTABLES
##############################################################################

add_executable(Engine ${CMAKE_CURRENT_SOURCE_DIR}/main_migration.cpp)
target_link_libraries(Engine SEISMIC-TOOLBOX SEISMIC-OPERATIONS)

add_executable(Modeller ${CMAKE_CURRENT_SOURCE_DIR}/main_modelling.cpp)
target_link_libraries(Modeller SEISMIC-TOOLBOX SEISMIC-OPERATIONS)

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
if (${BUILD_TESTS})
    message(STATUS "Building Seismic Toolbox Tests")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
    include(CTest)
    add_test(NAME SEISMIC-TOOLBOX-TESTS
            COMMAND seismic-toolbox-tests
            )
endif ()

############################################################CXX##################
# TOOLS
##############################################################################

option(BUILD_TOOLS "Option to enable building tools" OFF)
if (BUILD_TOOLS)
    message(STATUS "Building Seismic Tools")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tools)
endif ()
