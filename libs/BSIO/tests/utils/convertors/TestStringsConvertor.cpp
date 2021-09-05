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

#include <bs/io/utils/convertors/StringsConvertor.hpp>

#include <cstring>

#include <prerequisites/libraries/catch/catch.hpp>


using namespace bs::io::utils::convertors;

void TEST_CASE_STRING_CONV() {
    const std::string s = "22";
    SECTION("String to Unsigned Short")
    {
        unsigned short t1 = 22;
        REQUIRE(StringsConvertor::ToULong(s) == t1);
    }

    SECTION("String to Int")
    {
        int t1 = 22;
        REQUIRE(StringsConvertor::ToInt(s) == t1);
    }

    SECTION("String to Long")
    {
        long t1 = 22;
        REQUIRE(StringsConvertor::ToLong(s) == t1);
    }

    SECTION("String to Unsigned Long")
    {
        unsigned long t1 = 22;
        REQUIRE(StringsConvertor::ToULong(s) == t1);
    }

    SECTION("EBCDIC_TO_ASCII_CHAR")
    {
        unsigned char c = '\x96';
        c = StringsConvertor::E2A(c);
        REQUIRE(c == 'o');
    }

    SECTION("EBCDIC_TO_ASCII")
    {
        unsigned char str[] = "\xc8\x85\x93\x93\x96\x40\xa3\x88\x85\x99\x85\x5a";
        StringsConvertor::E2A(str, 12);
        REQUIRE(strcmp((const char *) str, "Hello there!") == 0);
    }
}


/**
 * REQUIRED TESTS:
 *
 * N.B. This test shall test only StringsConvertor.
 *
 * 1. All functions individually to return desired output in same test case yet different sections,
 *    also different permutations of configuration map shall be used to assure correct variations.
 *
 * 2. RC values -if any- to be tested and required against its ground truth
 *    (i.e. both failure and success cases shall be tested)
 */

TEST_CASE("Strings Convertor") {
TEST_CASE_STRING_CONV();

}
