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


add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/components)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/data-units)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/engines)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/helpers)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/utils)


set(OPERATIONS-LIBS

        BS-BASE
        BS-TIMER
        BS-IO
        FILE-COMPRESSION
        ${OPERATIONS-LIBS}
        )

add_library(SEISMIC-OPERATIONS STATIC ${OPERATIONS-SOURCES})
target_link_libraries(SEISMIC-OPERATIONS ${OPERATIONS-LIBS})
