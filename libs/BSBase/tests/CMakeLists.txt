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


set(BS_BASE_TESTFILES ${CMAKE_CURRENT_SOURCE_DIR}/test_main.cpp)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/backend)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/configurations)

enable_testing()
add_executable(bs-base-tests ${BS_BASE_TESTFILES})
target_link_libraries(bs-base-tests BS-BASE)
