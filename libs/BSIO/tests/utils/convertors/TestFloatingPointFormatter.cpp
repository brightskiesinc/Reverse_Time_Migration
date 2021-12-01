/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of BS I/O.
 *
 * BS I/O is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BS I/O is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */

#include <prerequisites/libraries/catch/catch.hpp>

#include <bs/io/utils/convertors/FloatingPointFormatter.hpp>


#define IBM_EPS 4.7683738e-7 /* Worst case error */

using namespace bs::io::utils::convertors;


void
TEST_FLOAT_FORMAT() {
    SECTION("Float Array Size") {
        REQUIRE(FloatingPointFormatter::GetFloatArrayRealSize(4, 1) == 16);
        REQUIRE(FloatingPointFormatter::GetFloatArrayRealSize(4, 2) == 16);
        REQUIRE(FloatingPointFormatter::GetFloatArrayRealSize(4, 3) == 8);
        REQUIRE(FloatingPointFormatter::GetFloatArrayRealSize(4, 8) == 4);
    }

    SECTION("To IBM") {
        float src[2] = {0.23, 1.234567};
        float dst[2];
        FloatingPointFormatter::Format((char *) src, (char *) dst, 2 * sizeof(float), 2, 1, 0);
        REQUIRE(dst[0] == 115316.500000);
        REQUIRE(dst[1] == -1573480.125000);
    }

    SECTION("From IBM") {
        float src[2] = {115316.500000, -1573480.125000};
        float dst[2];
        FloatingPointFormatter::Format((char *) src, (char *) dst, 2 * sizeof(float), 2, 1, 1);
        REQUIRE(((dst[0] < 0.23 + IBM_EPS) && (dst[0] > 0.23 - IBM_EPS)));
        REQUIRE(((dst[1] < 1.234567 + IBM_EPS) && (dst[1] > 1.234567 - IBM_EPS)));
    }
}


/**
 * REQUIRED TESTS:
 *
 * N.B. This test shall test only FloatingPointFormatter.
 *
 * 1. All functions individually to return desired output in same test case yet different sections,
 *    also different permutations of configuration map shall be used to assure correct variations.
 *
 * 2. RC values -if any- to be tested and required against its ground truth
 *    (i.e. both failure and success cases shall be tested)
 */

TEST_CASE("Floating Point Formatter") {
    TEST_FLOAT_FORMAT();
}
