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


if (USE_OMP)
    set(STBX-SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/concrete/computation-parameters/omp/computation_parameters_generator.cpp
            ${STBX-SOURCES}
            )
elseif (USE_DPC)
    set(STBX-SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/concrete/computation-parameters/oneapi/computation_parameters_generator.cpp
            ${STBX-SOURCES}
            )
elseif (USE_OMP_OFFLOAD)
    set(STBX-SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/concrete/computation-parameters/omp-offload/computation_parameters_generator.cpp
            ${STBX-SOURCES}
            )
endif ()

set(STBX-SOURCES

        ${CMAKE_CURRENT_SOURCE_DIR}/Generator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/primitive/CallbacksGenerator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/primitive/ComponentsGenerator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/primitive/ConfigurationsGenerator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/primitive/ComputationParametersGetter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/primitive/ComponentsGenerator.cpp

        ${STBX-SOURCES}
        PARENT_SCOPE
        )