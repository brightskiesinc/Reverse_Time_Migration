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

#include <bs/io/data-units/concrete/Gather.hpp>
#include <bs/io/test-utils/DataGenerator.hpp>

using namespace std;
using namespace bs::io::dataunits;
using namespace bs::io::testutils;


void
TEST_GATHER() {
    vector<Trace *> traces = DataGenerator::GenerateTraceVector(0, 10, 1);

    SECTION("Constructors") {
        unordered_map<TraceHeaderKey, string> unique_keys;
        unique_keys[TraceHeaderKey::NS] = "0";

        Gather g1(unique_keys, traces);
        REQUIRE(g1.GetNumberTraces() == 10);
        REQUIRE(g1.GetUniqueKeyValue<int>(TraceHeaderKey::NS) == 0);

        Gather g2(TraceHeaderKey::NS, "0", traces);
        REQUIRE(g1.GetNumberTraces() == 10);
        REQUIRE(g1.GetUniqueKeyValue<int>(TraceHeaderKey::NS) == 0);

        Gather g3(unique_keys);
        REQUIRE(g1.GetUniqueKeyValue<int>(TraceHeaderKey::NS) == 0);
    }

    SECTION("Add/Remove Trace") {
        unordered_map<TraceHeaderKey, string> unique_keys;
        unique_keys[TraceHeaderKey::NS] = "0";
        Gather g(unique_keys, traces);
        Trace trace(0);
        Trace *t = &trace;

        g.AddTrace(t);
        REQUIRE(g.GetNumberTraces() == 11);

        g.RemoveTrace(1);
        REQUIRE(g.GetNumberTraces() == 10);
    }

    SECTION("Sorting") {
        unordered_map<TraceHeaderKey, string> unique_keys;
        unique_keys[TraceHeaderKey::NS] = "0";
        Gather g(unique_keys, traces);
        vector<pair<TraceHeaderKey, Gather::SortDirection>> sorting_keys = {
                {TraceHeaderKey::FLDR, Gather::DES}
        };
        g.SortGather(sorting_keys);
        vector<Trace *> all_traces = g.GetAllTraces();
        int fldr = all_traces[0]->GetTraceHeaderKeyValue<int>(TraceHeaderKey::FLDR);
        int temp;
        for (auto t : all_traces) {
            temp = t->GetTraceHeaderKeyValue<int>(TraceHeaderKey::FLDR);
            REQUIRE(temp <= fldr);
            fldr = temp;
        }
    }
}


/**
 * REQUIRED TESTS:
 *
 * 1. All functions individually to return desired output in same test case yet different sections.
 * 2. Copy constructor to copy correctly.
 * 3. Move constructor to move correctly.
 * 4. Logical operators to compare correctly.
 */

TEST_CASE("GatherTest", "[Gather]") {
    TEST_GATHER();
}
