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

#include <bs/timer/test-utils/FunctionsGenerator.hpp>

#ifdef USE_OMP

#include "concrete/omp/OmpFunctionsGenerator.hpp"
#include "concrete/serial/SerialFunctionsGenerator.hpp"

#else

#include "concrete/serial/SerialFunctionsGenerator.hpp"

#endif


void
bs::timer::testutils::target_technology_test_function() {
#ifdef USE_OMP
    omp_test_function();
#else
    serial_test_function();
#endif
}

void
bs::timer::testutils::test_function() {
    serial_test_function();
}