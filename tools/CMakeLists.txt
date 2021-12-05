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


# Set minimum cmake version.
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
cmake_policy(SET CMP0048 NEW)

# Project settings.
project(Seismic-Tools LANGUAGES CXX VERSION 1.0.0)

if (CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
endif ()

# Set C++ standard to C++11 to allow for the advanced features used in code.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# CSV COMPARATOR EXECUTABLE
add_executable(Comparator ${CMAKE_CURRENT_SOURCE_DIR}/cmp/Comparator.cpp)


# SYNTHETIC MODELS GENERATOR EXECUTABLE
if (USE_OpenCV AND USE_OMP)
    add_executable(Generator ${CMAKE_CURRENT_SOURCE_DIR}/gen/SyntheticModelGenerator.cpp)
    target_link_libraries(Generator SEISMIC-OPERATIONS)
endif ()
