/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of Thoth (I/O Library).
 *
 * Thoth (I/O Library) is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Thoth (I/O Library) is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */

#include <bs/io/utils/checkers/Checker.hpp>

#include <prerequisites/libraries/catch/catch.hpp>

using namespace bs::io::utils::checkers;


void
TEST_CASE_CHECKER() {
    SECTION("Testing Endianness Of Machine")
    {
        unsigned int i = 1;
        char *c = (char *) &i;
        bool little = (*c);
        REQUIRE(Checker::IsLittleEndianMachine() == little);
    }
}


/**
 * REQUIRED TESTS:
 *
 * N.B. This test shall test only Checker.
 *
 * 1. All functions individually to return desired output in same test case yet different sections,
 *    also different permutations of configuration map shall be used to assure correct variations.
 *
 * 2. RC values -if any- to be tested and required against its ground truth
 *    (i.e. both failure and success cases shall be tested)
 */

TEST_CASE("Checker") {
TEST_CASE_CHECKER();

}