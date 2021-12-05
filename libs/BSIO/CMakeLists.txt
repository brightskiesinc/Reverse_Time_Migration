# Copyright (C) 2021 by Brightskies inc
#
# This file is part of BS I/O.
#
# BS I/O is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# BS I/O is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.


# Set minimum cmake version
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
cmake_policy(SET CMP0048 NEW)

# Project settings.
project(BSIO LANGUAGES CXX VERSION 1.0.0)

# Set cmake path module.
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

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

# Adding libraries subdirectories
get_directory_property(CMAKE_PARENT_DIR PARENT_DIRECTORY)
if (NOT CMAKE_PARENT_DIR)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/BSBase/include)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/BSBase)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/BSBase)
endif ()

# Adding the headers directories needed for all projects.
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/prerequisites)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

option(USE_OpenCV "Option to enable using OpenCV technology" OFF)
if (${USE_OpenCV})
    message(STATUS "Using OpenCV technology")
    find_package(OpenCV REQUIRED)
    include_directories(${OpenCV_INCLUDE_DIRS})
    add_definitions(-DUSING_OPENCV)
    list(APPEND LIBS "${OpenCV_LIBS}")
else ()
    message(STATUS "No OpenCV used")
endif ()

# Adding subdirectories
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

option(BUILD_EXAMPLES "Option to enable building example programs" OFF)
if (${BUILD_EXAMPLES})
    message(STATUS "Building BSIO Examples")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples)
endif ()

option(BUILD_TESTS "Option to enable building tests" OFF)
if (${BUILD_TESTS})
    message(STATUS "Building BSIO Tests")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
    include(CTest)
    add_test(NAME BS-IO-TESTS
            COMMAND bs-io-tests
            )
endif ()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mains)
