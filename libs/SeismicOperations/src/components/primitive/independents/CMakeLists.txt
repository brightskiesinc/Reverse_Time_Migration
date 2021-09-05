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

        # COMPUTATION KERNELS
        ${CMAKE_CURRENT_SOURCE_DIR}/computation-kernels/isotropic/SecondOrderComputationKernel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/computation-kernels/isotropic/StaggeredComputationKernel.cpp

        # MIGRATION ACCOMMODATORS
        ${CMAKE_CURRENT_SOURCE_DIR}/migration-accommodators/CrossCorrelationKernel.cpp

        # BOUNDARIES COMPONENTS
        ${CMAKE_CURRENT_SOURCE_DIR}/boundary-managers/extensions/Extension.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/boundary-managers/NoBoundaryManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/boundary-managers/RandomBoundaryManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/boundary-managers/SpongeBoundaryManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/boundary-managers/CPMLBoundaryManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/boundary-managers/StaggeredCPMLBoundaryManager.cpp

        # FORWARD COLLECTORS
        ${CMAKE_CURRENT_SOURCE_DIR}/forward-collectors/ReversePropagation.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/forward-collectors/TwoPropagation.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/forward-collectors/boundary-saver/BoundarySaver.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/forward-collectors/file-handler/file_handler.cpp

        # FORWARD COLLECTORS' HELPERS
        ${CMAKE_CURRENT_SOURCE_DIR}/forward-collectors/file-handler/file_handler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/forward-collectors/boundary-saver/BoundarySaver.cpp

        # TRACE MANAGERS
        ${CMAKE_CURRENT_SOURCE_DIR}/trace-managers/SeismicTraceManager.cpp

        # SOURCE INJECTORS
        ${CMAKE_CURRENT_SOURCE_DIR}/source-injectors/RickerSourceInjector.cpp

        # MODEL HANDLERS
        ${CMAKE_CURRENT_SOURCE_DIR}/model-handlers/SeismicModelHandler.cpp

        # TRACE WRITERS
        ${CMAKE_CURRENT_SOURCE_DIR}/trace-writers/SeismicTraceWriter.cpp

        ${OPERATIONS-SOURCES}
        PARENT_SCOPE
        )