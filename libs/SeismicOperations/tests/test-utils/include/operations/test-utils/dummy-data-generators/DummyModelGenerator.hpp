//
// Created by marwan-elsafty on 03/02/2021.
//


#ifndef OPERATIONS_LIB_TEST_UTILS_DUMMY_MODEL_GENERATOR_HPP
#define OPERATIONS_LIB_TEST_UTILS_DUMMY_MODEL_GENERATOR_HPP

#include <string>

namespace operations {
    namespace testutils {
        /**
         * @brief Generates a dummy *.segy file of the following parameters:
         *
         * Grid Size:-
         * nx := 5
         * ny := 5
         * nz := 5
         * nt := 1
         *
         * Cell Dimensions:-
         * dx := 6.25
         * dy := 6.25
         * dz := 6.25
         * dt := 1.0
         *
         * Model:-
         * Name = OPERATIONS_TEST_DATA_PATH "/<model-name>.segy"
         * i.e. OPERATIONS_TEST_DATA_PATH is a CMake predefined definition.
         * Size = nx * ny * nz;
         * Data = All filled with '1's
         */
        float *generate_dummy_model(const std::string &aFileName);

    } //namespace testutils
} //namespace operations

#endif //OPERATIONS_LIB_TEST_UTILS_COMPUTATION_PARAMETERS_DUMMY_MODEL_GENERATOR_HPP
