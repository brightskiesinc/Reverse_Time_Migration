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

#include <bs/io/data-units/data-types/TraceHeaderKey.hpp>

#include <prerequisites/libraries/catch/catch.hpp>

using namespace bs::io::dataunits;
using std::vector;


void
TEST_TRACE_HDR() {
    SECTION("Constructor")
    {
        TraceHeaderKey hdr(TraceHeaderKey::Key::FLDR);
        REQUIRE(hdr.GetKey() == TraceHeaderKey::Key::FLDR);
    }

    SECTION("Copy Constructor")
    {
        TraceHeaderKey hdr(2);
        REQUIRE(hdr.GetKey() == TraceHeaderKey::Key::FLDR);
    }

    SECTION("Logical Operators")
    {
        TraceHeaderKey hdr1(TraceHeaderKey::Key::FLDR);
        TraceHeaderKey hdr2(2);
        TraceHeaderKey hdr3(TraceHeaderKey::Key::TRACL);

        REQUIRE(hdr1 == hdr2);
        REQUIRE(hdr1 != hdr3);
        REQUIRE(hdr1 > hdr3);
        REQUIRE(hdr3 < hdr1);
    }

    SECTION("To String")
    {
        TraceHeaderKey hdr1(TraceHeaderKey::Key::FLDR);
        TraceHeaderKey hdr2(TraceHeaderKey::Key::TRACL);
        vector<TraceHeaderKey> keys;
        keys.push_back(hdr1);
        keys.push_back(hdr2);
        REQUIRE(TraceHeaderKey::GatherKeysToString(keys) == "FLDR_TRACL_");
        vector<std::string> values;
        values.push_back("1");
        values.push_back("10");
        REQUIRE(TraceHeaderKey::GatherValuesToString(values) == "1_10_");
        vector<std::string> res = TraceHeaderKey::StringToGatherValues("1_10_");
        REQUIRE(res[0] == "1");
        REQUIRE(res[1] == "10");

        REQUIRE(hdr1.ToString() == "FLDR");
    }
}


/**
 * REQUIRED TESTS:
 * 1. All functions individually to return desired output in same test case yet different sections.
 * 2. Copy constructor to copy correctly.
 * 3. Move constructor to move correctly.
 * 4. Logical operators to compare correctly.
 */

TEST_CASE("TraceHeaderKey") {
    TEST_TRACE_HDR();

}
