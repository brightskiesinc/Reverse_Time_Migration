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
#include "operations/components/independents/concrete/computation-kernels/isotropic/StaggeredComputationKernel.hpp"

#include "operations/components/dependents/concrete/memory-handlers/WaveFieldsMemoryHandler.hpp"
#include <bs/base/exceptions/concrete/NotImplementedException.hpp>
#include "operations/components/independents/concrete/computation-kernels/BaseComputationHelpers.hpp"

using namespace std;
using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;

FORWARD_DECLARE_COMPUTE_TEMPLATE(StaggeredComputationKernel, ComputePressure)

FORWARD_DECLARE_COMPUTE_TEMPLATE(StaggeredComputationKernel, ComputeVelocity)


template<KERNEL_MODE KERNEL_MODE_, bool IS_2D_, HALF_LENGTH HALF_LENGTH_>
void StaggeredComputationKernel::ComputePressure() {
    throw bs::base::exceptions::NOT_IMPLEMENTED_EXCEPTION();
}

template<KERNEL_MODE KERNEL_MODE_, bool IS_2D_, HALF_LENGTH HALF_LENGTH_>
void StaggeredComputationKernel::ComputeVelocity() {
    throw bs::base::exceptions::NOT_IMPLEMENTED_EXCEPTION();
}

void StaggeredComputationKernel::PreprocessModel() {
    throw bs::base::exceptions::NOT_IMPLEMENTED_EXCEPTION();
}
