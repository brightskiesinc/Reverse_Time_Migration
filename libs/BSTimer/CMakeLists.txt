# Copyright (C) 2021 by Brightskies inc
#
# This file is part of BS Timer.
#
# BS Timer is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# BS Timer is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.


# Set minimum cmake version.
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

# Add definitions.
if ("${BS_TIMER_WRITE_PATH}" STREQUAL "")
    SET(BS_TIMER_WRITE_PATH "./results")
endif ()
if ("${BS_TIMER_WORKLOAD_PATH}" STREQUAL "")
    SET(BS_TIMER_WORKLOAD_PATH "./workloads")
endif ()

if (USE_OMP)
    get_directory_property(CMAKE_PARENT_DIR PARENT_DIRECTORY)
    if (NOT CMAKE_PARENT_DIR)
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
    endif ()
    add_definitions(-DUSE_OMP)
    message(STATUS "Building BSTimer (OpenMP Version)")
else ()
    message(STATUS "Building BSTimer (Serial Version)")
endif ()

# Replace definitions with acquired ones from config.sh script if exists.
add_definitions(
        -DBS_TIMER_WRITE_PATH="${BS_TIMER_WRITE_PATH}"
        -DBS_TIMER_WORKLOAD_PATH="${BS_TIMER_WORKLOAD_PATH}")


project(BSTimer LANGUAGES CXX VERSION 1.0.0)

# Set C++ standard to C++17 to allow for the advanced features used in code.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add definitions.
if ("${BS_TIMER_WRITE_PATH}" STREQUAL "")
    SET(BS_TIMER_WRITE_PATH "./results")
endif ()
if ("${BS_TIMER_WORKLOAD_PATH}" STREQUAL "")
    SET(BS_TIMER_WORKLOAD_PATH "./workloads")
endif ()

# Replace definitions with acquired ones from config.sh script if exists.
add_definitions(
        -DBS_TIMER_WRITE_PATH="${BS_TIMER_WRITE_PATH}"
        -DBS_TIMER_WORKLOAD_PATH="${BS_TIMER_WORKLOAD_PATH}")

# Adding libraries subdirectories
get_directory_property(CMAKE_PARENT_DIR PARENT_DIRECTORY)
if (NOT CMAKE_PARENT_DIR)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/BSBase/include)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/BSBase)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/BSBase)
endif ()

# Adding the headers directories needed for all projects.
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Adding subdirectories
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

option(BUILD_EXAMPLES "Option to enable building example programs" OFF)
if (${BUILD_EXAMPLES})
    message(STATUS "Building BSTimer Examples")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples)
endif ()

option(BUILD_TESTS "Option to enable building tests" OFF)
if (${BUILD_TESTS})
    message(STATUS "Building BSTimer Tests")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
    include(CTest)
    add_test(NAME BS-TIMER-TESTS
            COMMAND bs-timer-tests
            )
endif ()
