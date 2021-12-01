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


add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/agents)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/generators)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/parsers)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/writers)

if (USE_MPI)
    add_definitions(-DUSING_MPI)
    set(STBX-LIBS SEISMIC-OPERATIONS ${MPI_LIBRARIES})
else ()
    set(STBX-LIBS SEISMIC-OPERATIONS)
endif ()


set(STBX-LIBS

        SEISMIC-OPERATIONS
        ${STBX-LIBS}
        )

add_library(SEISMIC-TOOLBOX STATIC ${STBX-SOURCES})
target_link_libraries(SEISMIC-TOOLBOX ${STBX-LIBS})

if (MPI_COMPILE_FLAGS)
    set_target_properties(SEISMIC-TOOLBOX PROPERTIES
            COMPILE_FLAGS ${MPI_COMPILE_FLAGS})
endif ()

if (MPI_LINK_FLAGS)
    set_target_properties(SEISMIC-TOOLBOX PROPERTIES
            LINK_FLAGS ${MPI_LINK_FLAGS})
endif ()