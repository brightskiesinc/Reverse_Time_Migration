# Copyright (C) 2021 by Brightskies inc
#
# This file is part of BS Base Package.
#
# BS Base Package is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# BS Base Package is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.


# Set minimum cmake version
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
cmake_policy(SET CMP0048 NEW)

# Intel Parallel Studio tools should be Sourced
# when using the cmake command or with the build script
if ("${USE_INTEL}" STREQUAL "")
    SET(USE_INTEL "NO")
endif ()

option(USE_OMP "Use OpenMP technology" OFF)
option(USE_DPC "Use DPC++ technology" OFF)
option(USE_OMP_OFFLOAD "Use OpenMP Offload technology" OFF)

if (${USE_OMP})
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
    add_definitions(-DUSING_OMP)
    message(STATUS "Building BSBase (OpenMP Version)")
elseif (${USE_DPC})
    get_directory_property(CMAKE_PARENT_DIR PARENT_DIRECTORY)
    if (NOT CMAKE_PARENT_DIR)
        set(CMAKE_C_COMPILER gcc)
        set(CMAKE_CXX_COMPILER dpcpp)
        set(CMAKE_CXX_FLAGS_DEBUG "-O3 -fsycl -std=c++17")
        set(CMAKE_CXX_FLAGS_RELEASE "-O3 -fsycl -std=c++17")
        set(CMAKE_CXX_FLAGS " -O3 -fsycl -std=c++17")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lOpenCL -lsycl")
    endif ()
    add_definitions(-DUSING_DPCPP)
    message(STATUS "Building BSBase (DPC++ Version)")
elseif (${USE_OMP_OFFLOAD})
    get_directory_property(CMAKE_PARENT_DIR PARENT_DIRECTORY)
    if (NOT CMAKE_PARENT_DIR)
        set(CMAKE_C_COMPILER gcc)
        set(CMAKE_CXX_COMPILER icpx)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fiopenmp -std=c++17 -fopenmp-targets=spir64 -O3 -D__STRICT_ANSI__  ")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fiopenmp -std=c++17 -fopenmp-targets=spir64 -O3 -D__STRICT_ANSI__  ")
        set(CMAKE_CXX_FLAGS "-fiopenmp -std=c++17 -fopenmp-targets=spir64 -O3 -D__STRICT_ANSI__ ")
    endif ()
    add_definitions(-DUSING_OMP_OFFLOAD)
    message(STATUS "Building BSBase (OpenMP Offload Version)")
else ()
    message(STATUS "Building BSBase (Serial Version)")
endif ()

# Project settings.
project(BSBase LANGUAGES CXX VERSION 1.0.0)

# Set C++ standard to C++17 to allow for the advanced features used in code.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Definitions
if ("${DATA_PATH}" STREQUAL "")
    SET(DATA_PATH "./data")
endif ()
if ("${WRITE_PATH}" STREQUAL "")
    SET(WRITE_PATH "./results")
endif ()
if ("${WORKLOAD_PATH}" STREQUAL "")
    SET(WORKLOAD_PATH "./workloads")
endif ()

add_definitions(
        -DDATA_PATH="${PROJECT_SOURCE_DIR}/${DATA_PATH}"
        -DWRITE_PATH="${PROJECT_SOURCE_DIR}/${WRITE_PATH}"
        -DWORKLOAD_PATH="${PROJECT_SOURCE_DIR}/${WORKLOAD_PATH}")

# Libraries settings.
find_package(spdlog)
if (spdlog_FOUND)
    message(STATUS "spdlog library is found")
    add_definitions(-DUSING_SPDLOG)
endif ()

# Adding the headers directories needed for all projects.
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Adding subdirectories
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

option(BUILD_EXAMPLES "Option to enable building example programs" OFF)
if (${BUILD_EXAMPLES})
    message(STATUS "Building Examples")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples)
endif ()

option(BUILD_TESTS "Option to enable building tests" OFF)
if (${BUILD_TESTS})
    message(STATUS "Building Tests")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
    include(CTest)
    add_test(NAME BS-BASE-TESTS
            COMMAND bs-base-tests
            )
endif ()
