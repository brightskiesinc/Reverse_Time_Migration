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

#include <bs/io/data-units/concrete/Gather.hpp>
#include <bs/io/test-utils/DataGenerator.hpp>

#include <prerequisites/libraries/catch/catch.hpp>

using namespace bs::io::dataunits;
using namespace bs::io::testutils;
using std::vector;


void
TEST_GATHER() {
    vector<Trace *> traces = DataGenerator::GenerateTraceVector(0, 10, 1);

    SECTION("Constructors")
    {
        std::unordered_map<TraceHeaderKey, std::string> aUniqueKeys;
        aUniqueKeys[TraceHeaderKey::NS] = "0";
        Gather g(aUniqueKeys, traces);
        REQUIRE(g.GetNumberTraces() == 10);
        REQUIRE(g.GetUniqueKeyValue<int>(TraceHeaderKey::NS) == 0);
        Gather g2(TraceHeaderKey::NS, "0", traces);
        REQUIRE(g.GetNumberTraces() == 10);
        REQUIRE(g.GetUniqueKeyValue<int>(TraceHeaderKey::NS) == 0);
        Gather g3(aUniqueKeys);
        REQUIRE(g.GetUniqueKeyValue<int>(TraceHeaderKey::NS) == 0);

    }

    SECTION("Add/Remove Trace")
    {
        std::unordered_map<TraceHeaderKey, std::string> aUniqueKeys;
        aUniqueKeys[TraceHeaderKey::NS] = "0";
        Gather g(aUniqueKeys, traces);
        Trace trce(0);
        Trace *tr = &trce;
        g.AddTrace(tr);
        REQUIRE(g.GetNumberTraces() == 11);
        g.RemoveTrace(1);
        REQUIRE(g.GetNumberTraces() == 10);

    }

    SECTION("Sorting")
    {
        std::unordered_map<TraceHeaderKey, std::string> aUniqueKeys;
        aUniqueKeys[TraceHeaderKey::NS] = "0";
        Gather g(aUniqueKeys, traces);
        vector<std::pair<TraceHeaderKey, Gather::SortDirection>> aSortingKeys = {{TraceHeaderKey::FLDR, Gather::DES}};
        g.SortGather(aSortingKeys);
        vector<Trace *> traces = g.GetAllTraces();
        int fldr = traces[0]->GetTraceHeaderKeyValue<int>(TraceHeaderKey::FLDR);
        int temp;
        for (auto tr: traces) {
            temp = tr->GetTraceHeaderKeyValue<int>(TraceHeaderKey::FLDR);
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
