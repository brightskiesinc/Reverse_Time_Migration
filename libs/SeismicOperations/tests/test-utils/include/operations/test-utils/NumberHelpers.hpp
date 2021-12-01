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


#ifndef OPERATIONS_LIB_TEST_UTILS_NUMBERS_HELPERS_HPP
#define OPERATIONS_LIB_TEST_UTILS_NUMBERS_HELPERS_HPP

#include <cmath>

namespace operations {
    namespace testutils {

#define OP_TU_TOLERANCE 0

        float calculate_norm(const float *mat, uint nx, uint nz, uint ny);

        bool approximately_equal(float a, float b, float tolerance = OP_TU_TOLERANCE);

        bool essentially_equal(float a, float b, float tolerance = OP_TU_TOLERANCE);

        bool definitely_greater_than(float a, float b, float tolerance = OP_TU_TOLERANCE);

        bool definitely_less_than(float a, float b, float tolerance = OP_TU_TOLERANCE);

    } //namespace testutils
} //namespace operations


#endif //OPERATIONS_LIB_TEST_UTILS_NUMBERS_HELPERS_HPP
