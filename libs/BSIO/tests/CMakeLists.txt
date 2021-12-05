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


add_definitions(-DIO_TESTS_RESULTS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/test-results/")
add_definitions(-DIO_TESTS_WORKLOADS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/test-files/")

set(BS_IO_TESTFILES ${CMAKE_CURRENT_SOURCE_DIR}/test_main.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/test-utils/include)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test-utils)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/test-utils/include)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test-utils/src)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/data-units)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/streams)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/utils)

enable_testing()
add_executable(bs-io-tests ${BS_IO_TESTFILES})
target_link_libraries(bs-io-tests BS-IO)
