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
#ifndef SEISMIC_TOOLBOX_GENERATORS_COMPUTATION_PARAMETERS_COMPUTATION_PARAMETERS_GENERATOR_H
#define SEISMIC_TOOLBOX_GENERATORS_COMPUTATION_PARAMETERS_COMPUTATION_PARAMETERS_GENERATOR_H

#include <operations/common/ComputationParameters.hpp>

#include <prerequisites/libraries/nlohmann/json.hpp>


operations::common::ComputationParameters *generate_parameters(nlohmann::json &aMap);

#endif // SEISMIC_TOOLBOX_GENERATORS_COMPUTATION_PARAMETERS_COMPUTATION_PARAMETERS_GENERATOR_H
