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
