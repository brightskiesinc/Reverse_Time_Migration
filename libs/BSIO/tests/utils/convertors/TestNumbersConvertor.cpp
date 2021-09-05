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

#include <bs/io/utils/convertors/NumbersConvertor.hpp>

#include <prerequisites/libraries/catch/catch.hpp>

using namespace bs::io::utils::convertors;


void
TEST_NUM_CONV() {
    /**
     * Please note that the comments in the NumbersConvertor.cpp file may have been copied incorrectly from the
     * FloatingPointFormatter.cpp file
     */

    SECTION("Short Int Little Endian")
    {
        short int x = 258;
        REQUIRE(NumbersConvertor::ToLittleEndian(x) == 513);
    }

    SECTION("Short Int Array Little Endian")
    {
        short int x[] = {258, 257, -2};
        short int *y = NumbersConvertor::ToLittleEndian(x, 3);
        short int g[] = {513, 257, -257};
        for (int i = 0; i < 3; i++) {
            REQUIRE(y[i] == g[i]);
        }
    }

    SECTION("Unsigned Short Int Little Endian")
    {
        unsigned short int x = 258;
        REQUIRE(NumbersConvertor::ToLittleEndian(x) == 513);
    }

    SECTION("Unsigned Short Int Array Little Endian")
    {
        unsigned short int x[] = {258, 257, 256};
        unsigned short int *y = NumbersConvertor::ToLittleEndian(x, 3);
        unsigned short int g[] = {513, 257, 1};
        for (int i = 0; i < 3; i++) {
            REQUIRE(y[i] == g[i]);
        }
    }

    SECTION("Int Little Endian")
    {
        int x = 1074266625;                   // 2^0 + 2^9 + 2^19 + 2^30
        REQUIRE(NumbersConvertor::ToLittleEndian(x) == 16910400);
    }

    SECTION("Int Array Little Endian")
    {
        int x[] = {1, 256, -2};
        int *y = NumbersConvertor::ToLittleEndian(x, 3);
        int g[] = {16777216, 65536, -16777217};
        for (int i = 0; i < 3; i++) {
            REQUIRE(y[i] == g[i]);
        }
    }

    SECTION("Unsigned Int Little Endian")
    {
        unsigned int x = 1074266625;                   // 2^0 + 2^9 + 2^19 + 2^30
        REQUIRE(NumbersConvertor::ToLittleEndian(x) == 16910400);
    }

    SECTION("Unsigned Int Array Little Endian")
    {
        unsigned int x[] = {1, 256, 16777216};
        unsigned int *y = NumbersConvertor::ToLittleEndian(x, 3);
        unsigned int g[] = {16777216, 65536, 1};
        for (int i = 0; i < 3; i++) {
            REQUIRE(y[i] == g[i]);
        }
    }

    SECTION("Float Little Endian")
    {
        float x = 1.1;
        auto g = (unsigned char *) &x;
        float y = NumbersConvertor::ToLittleEndian(x);
        auto p = (unsigned char *) &y;
        for (int i = 0; i < sizeof(float); i++) {
            REQUIRE(p[i] == g[sizeof(float) - i - 1]);
        }
    }

    SECTION("Float Array Little Endian")
    {
        float x[] = {1.1, 2.0, -3.1};
        float temp[3];
        memcpy(temp, x, 3 * sizeof(float));
        unsigned char *g[3];
        g[0] = (unsigned char *) &(x[0]);
        g[1] = (unsigned char *) &(x[1]);
        g[2] = (unsigned char *) &(x[2]);

        float *y = NumbersConvertor::ToLittleEndian(temp, 3);
        unsigned char *p[3];
        p[0] = (unsigned char *) &(y[0]);
        p[1] = (unsigned char *) &(y[1]);
        p[2] = (unsigned char *) &(y[2]);

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < sizeof(float); j++) {
                REQUIRE(p[i][j] == g[i][sizeof(float) - j - 1]);
            }
        }
    }

    SECTION("Signed Char Little Endian")
    {
        signed char x = 'O';
        REQUIRE(NumbersConvertor::ToLittleEndian(x) == 'O');
    }

    SECTION("Signed Char Array Little Endian")
    {
        signed char x[] = {'O', 'P', 'Q', 'R'};
        signed char g[] = {'O', 'P', 'Q', 'R'};
        signed char *y = NumbersConvertor::ToLittleEndian(x, 4);
        for (int i = 0; i < 4; i++) {
            REQUIRE(y[i] == g[i]);
        }
    }

    SECTION("To Little Endian")
    {
        int x[] = {1, 256, -2};
        char *temp = (char *) x;
        int g_int[] = {16777216, 65536, -16777217};
        int *y_int = (int *) NumbersConvertor::ToLittleEndian(temp, 3, 1);

        for (int i = 0; i < 3; i++) {
            REQUIRE(y_int[i] == g_int[i]);
        }

        short int k = 258;
        auto z = (short int *) NumbersConvertor::ToLittleEndian((char *) &k, 1, 0);
        REQUIRE(*z == 258);

        auto y_short = (short int *) NumbersConvertor::ToLittleEndian((char *) &k, 1, 3);
        REQUIRE(*y_short == 513);
    }
}


/**
 * REQUIRED TESTS:
 *
 * N.B. This test shall test only NumbersConvertor.
 *
 * 1. All functions individually to return desired output in same test case yet different sections,
 *    also different permutations of configuration map shall be used to assure correct variations.
 *
 * 2. RC values -if any- to be tested and required against its ground truth
 *    (i.e. both failure and success cases shall be tested)
 */

TEST_CASE("Numbers Convertor") {
TEST_NUM_CONV();

}
