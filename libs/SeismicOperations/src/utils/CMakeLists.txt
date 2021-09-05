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


set(OPERATIONS-SOURCES

        ${CMAKE_CURRENT_SOURCE_DIR}/filters/noise_filtering.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/interpolation/Interpolator.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/io/read_utils.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/io/write_utils.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/sampling/Sampler.cpp

        ${OPERATIONS-SOURCES}
        PARENT_SCOPE
        )

set(OPERATIONS-LIBS

        ${OPERATIONS-LIBS}
        PARENT_SCOPE
        )

add_subdirectory(compressor)
