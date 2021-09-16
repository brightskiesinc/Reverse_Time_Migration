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

#include <bs/io/utils/convertors/KeysConvertor.hpp>

#include <prerequisites/libraries/catch/catch.hpp>
#include <iostream>

using namespace bs::io::utils::convertors;
using namespace bs::io::dataunits;
using std::string;


void
TEST_KEY_CONV() {
    SECTION("ToTraceHeaderKeyTest")
    {
        string temp("FLDR");
        REQUIRE(KeysConvertor::ToTraceHeaderKey(temp) == TraceHeaderKey::FLDR);

        string temp1("ns");
        REQUIRE(KeysConvertor::ToTraceHeaderKey(temp1) == TraceHeaderKey::NS);

        string temp2("mInUtE");
        REQUIRE(KeysConvertor::ToTraceHeaderKey(temp2) == TraceHeaderKey::MINUTE);

        string temp3("foo");
        REQUIRE_THROWS(KeysConvertor::ToTraceHeaderKey(temp3) != TraceHeaderKey::NS);
    }
}


/**
 * REQUIRED TESTS:
 *
 * N.B. This test shall test only KeysConvertor.
 *
 * 1. All functions individually to return desired output in same test case yet different sections,
 *    also different permutations of configuration map shall be used to assure correct variations.
 *
 * 2. RC values -if any- to be tested and required against its ground truth
 *    (i.e. both failure and success cases shall be tested)
 */

TEST_CASE("Keys Convertor") {
    TEST_KEY_CONV();

}