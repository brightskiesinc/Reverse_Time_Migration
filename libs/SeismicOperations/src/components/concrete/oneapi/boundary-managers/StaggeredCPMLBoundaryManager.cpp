/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of SeismicToolbox.
 *
 * SeismicToolbox is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SeismicToolbox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */
#include "operations/components/independents/concrete/boundary-managers/StaggeredCPMLBoundaryManager.hpp"
#include <bs/base/exceptions/concrete/NotImplementedException.hpp>
#include <operations/components/independents/concrete/computation-kernels/BaseComputationHelpers.hpp>

using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;

FORWARD_DECLARE_BOUND_TEMPLATE(StaggeredCPMLBoundaryManager::CalculateVelocityFirstAuxiliary)

FORWARD_DECLARE_BOUND_TEMPLATE(StaggeredCPMLBoundaryManager::CalculateVelocityCPMLValue)

FORWARD_DECLARE_BOUND_TEMPLATE(StaggeredCPMLBoundaryManager::CalculatePressureFirstAuxiliary)

FORWARD_DECLARE_BOUND_TEMPLATE(StaggeredCPMLBoundaryManager::CalculatePressureCPMLValue)

template<bool ADJOINT_, int DIRECTION_, bool OPPOSITE_, int HALF_LENGTH_>
void StaggeredCPMLBoundaryManager::CalculateVelocityFirstAuxiliary() {
    throw bs::base::exceptions::NOT_IMPLEMENTED_EXCEPTION();
}

template<bool ADJOINT_, int DIRECTION_, bool OPPOSITE_, int HALF_LENGTH_>
void StaggeredCPMLBoundaryManager::CalculateVelocityCPMLValue() {
    throw bs::base::exceptions::NOT_IMPLEMENTED_EXCEPTION();
}

template<bool ADJOINT_, int DIRECTION_, bool OPPOSITE_, int HALF_LENGTH_>
void StaggeredCPMLBoundaryManager::CalculatePressureFirstAuxiliary() {
    throw bs::base::exceptions::NOT_IMPLEMENTED_EXCEPTION();
}

template<bool ADJOINT_, int DIRECTION_, bool OPPOSITE_, int HALF_LENGTH_>
void StaggeredCPMLBoundaryManager::CalculatePressureCPMLValue() {
    throw bs::base::exceptions::NOT_IMPLEMENTED_EXCEPTION();
}
