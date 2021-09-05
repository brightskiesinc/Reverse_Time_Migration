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


#ifndef OPERATIONS_LIB_TEST_UTILS_COMPUTATION_PARAMETERS_DUMMY_PARAMETERS_GENERATOR_HPP
#define OPERATIONS_LIB_TEST_UTILS_COMPUTATION_PARAMETERS_DUMMY_PARAMETERS_GENERATOR_HPP

#include <operations/common/ComputationParameters.hpp>
#include <operations/common/DataTypes.h>
#include <operations/test-utils/TestEnums.hpp>

namespace operations {
    namespace testutils {

        common::ComputationParameters *
        generate_computation_parameters(
                OP_TU_WIND aWindow, APPROXIMATION aApproximation = ISOTROPIC);

        common::ComputationParameters *generate_average_case_parameters();

    } //namespace testutils
} //namespace operations

#endif //OPERATIONS_LIB_TEST_UTILS_COMPUTATION_PARAMETERS_DUMMY_PARAMETERS_GENERATOR_HPP
