/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of BS Timer.
 *
 * BS Timer is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BS Timer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */

#include "OmpFunctionsGenerator.hpp"

#include <cmath>

#define BS_TIMER_TESTS_ARRAY_SIZE   1000    /* Test array size definition. */

void
omp_internal_test_function() {
    float array[BS_TIMER_TESTS_ARRAY_SIZE][BS_TIMER_TESTS_ARRAY_SIZE];
#pragma omp parallel for
    for (int i = 0; i < BS_TIMER_TESTS_ARRAY_SIZE; i++) {
        for (int j = 0; j < BS_TIMER_TESTS_ARRAY_SIZE; j++) {
            array[i][j] = sqrt(i) * sqrt(j) * sqrt(i) * sqrt(j);
        }
    }
}

void
bs::timer::testutils::omp_test_function() {
    omp_internal_test_function();
}
