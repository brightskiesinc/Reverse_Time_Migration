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