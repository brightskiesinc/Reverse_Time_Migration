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


set(OPERATIONS-TESTFILES

        # BOUNDARY MANAGERS
        ${CMAKE_CURRENT_SOURCE_DIR}/boundary-managers/TestCPMLBoundaryManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/boundary-managers/TestNoneBoundaryManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/boundary-managers/TestSpongeBoundaryManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/boundary-managers/TestRandomBoundaryManager.cpp

        # MODEL HANDLERS
        ${CMAKE_CURRENT_SOURCE_DIR}/model-handlers/TestSeismicModelHandler.cpp

        # MIGRATION ACCOMODATORS
        ${CMAKE_CURRENT_SOURCE_DIR}/migration-accommodators/TestCrossCorrelationKernel.cpp

        # TRACE MANAGERS
        ${CMAKE_CURRENT_SOURCE_DIR}/trace-managers/TestSeismicTraceManager.cpp

        # SOURCE INJECTORS
        ${CMAKE_CURRENT_SOURCE_DIR}/source-injectors/TestRickerSourceInjector.cpp

        # COMPUTATION KERNELS
        ${CMAKE_CURRENT_SOURCE_DIR}/computation-kernels/iso/TestSecondOrderComputationKernel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/computation-kernels/iso/TestStaggeredComputationKernel.cpp

        # FORWARD COLLECTORS
        ${CMAKE_CURRENT_SOURCE_DIR}/forward-collectors/TestReversePropagation.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/forward-collectors/TestTwoPropagation.cpp


        ${OPERATIONS-TESTFILES}
        PARENT_SCOPE
        )
