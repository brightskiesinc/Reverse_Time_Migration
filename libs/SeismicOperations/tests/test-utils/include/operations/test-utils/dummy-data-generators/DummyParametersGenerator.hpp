//
// Created by marwan on 11/01/2021.
//

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
