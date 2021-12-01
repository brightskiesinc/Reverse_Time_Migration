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

        # DATA GENERATORS
        ${CMAKE_CURRENT_SOURCE_DIR}/dummy-data-generators/DummyGridBoxGenerator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/dummy-data-generators/DummyConfigurationMapGenerator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/dummy-data-generators/DummyParametersGenerator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/dummy-data-generators/DummyModelGenerator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/dummy-data-generators/DummyTraceGenerator.cpp

        # NORM CALCULATOR
        ${CMAKE_CURRENT_SOURCE_DIR}/NumberHelpers.cpp

        # ENVIRONMENT HANDLERS
        ${CMAKE_CURRENT_SOURCE_DIR}/EnvironmentHandler.cpp

        ${OPERATIONS-TESTFILES}
        PARENT_SCOPE
        )