//
// Created by amr-nasr on 12/12/2019.
//

#ifndef SEISMIC_TOOLBOX_GENERATORS_COMPUTATION_PARAMETERS_COMPUTATION_PARAMETERS_GENERATOR_H
#define SEISMIC_TOOLBOX_GENERATORS_COMPUTATION_PARAMETERS_COMPUTATION_PARAMETERS_GENERATOR_H

#include <operations/common/ComputationParameters.hpp>

#include <libraries/nlohmann/json.hpp>

operations::common::ComputationParameters *generate_parameters(nlohmann::json &map);

#endif // SEISMIC_TOOLBOX_GENERATORS_COMPUTATION_PARAMETERS_COMPUTATION_PARAMETERS_GENERATOR_H
