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

#ifndef OPERATIONS_LIB_COMPONENTS_COMPONENTS_HPP
#define OPERATIONS_LIB_COMPONENTS_COMPONENTS_HPP

/// COMPUTATION KERNELS
#include <operations/components/independents/concrete/computation-kernels/isotropic/SecondOrderComputationKernel.hpp>
#include <operations/components/independents/concrete/computation-kernels/isotropic/StaggeredComputationKernel.hpp>

/// MIGRATION ACCOMMODATORS
#include <operations/components/independents/concrete/migration-accommodators/CrossCorrelationKernel.hpp>

/// BOUNDARIES COMPONENTS
#include <operations/components/independents/concrete/boundary-managers/NoBoundaryManager.hpp>
#include <operations/components/independents/concrete/boundary-managers/RandomBoundaryManager.hpp>
#include <operations/components/independents/concrete/boundary-managers/SpongeBoundaryManager.hpp>
#include <operations/components/independents/concrete/boundary-managers/CPMLBoundaryManager.hpp>
#include <operations/components/independents/concrete/boundary-managers/StaggeredCPMLBoundaryManager.hpp>

/// FORWARD COLLECTORS
#include <operations/components/independents/concrete/forward-collectors/ReversePropagation.hpp>
#include <operations/components/independents/concrete/forward-collectors/TwoPropagation.hpp>

/// TRACE MANAGERS
#include <operations/components/independents/concrete/trace-managers/SeismicTraceManager.hpp>

///  SOURCE INJECTORS
#include <operations/components/independents/concrete/source-injectors/RickerSourceInjector.hpp>

/// MEMORY HANDLERS
#include <operations/components/dependents/concrete/memory-handlers/WaveFieldsMemoryHandler.hpp>

/// MODEL HANDLERS
#include <operations/components/independents/concrete/model-handlers/SeismicModelHandler.hpp>

/// TRACE WRITERS
#include <operations/components/independents/concrete/trace-writers/SeismicTraceWriter.hpp>


#endif //OPERATIONS_LIB_COMPONENTS_COMPONENTS_HPP
