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

#include <bs/io/data-units/concrete/Trace.hpp>

#include <prerequisites/libraries/catch/catch.hpp>
#include <iostream>

using namespace bs::io::dataunits;


void
TEST_TRACE() {
    SECTION("Constructor")
    {
        Trace tr(10);
        REQUIRE(tr.GetTraceHeaderKeyValue<int>(TraceHeaderKey(TraceHeaderKey::Key::NS)) == 10);
    }

    SECTION("Move Constructor")
    {
        Trace tr(3);
        auto data = (float *) malloc(3 * sizeof(float));
        data[0] = 1.0;
        data[1] = 2.3;
        data[2] = 5.6;
        tr.SetTraceData(data);
        Trace temp = std::move(tr);
        float *ret2 = temp.GetTraceData();
        float x = 2.3;
        REQUIRE(ret2[1] == x);
    }

    SECTION("HasHeader")
    {
        Trace tr(3);
        REQUIRE(tr.HasTraceHeader(TraceHeaderKey::NS));
        REQUIRE(!tr.HasTraceHeader(TraceHeaderKey::TRACL));
        REQUIRE(tr.GetNumberOfSamples() == 3);
        REQUIRE(tr.GetTraceHeaders()->size() == 1);
    }

    SECTION("Set_Scaled_Coordinate")
    {
        Trace tr(3);
        tr.SetScaledCoordinateHeader(TraceHeaderKey(TraceHeaderKey::SCALCO), 2.0);
        float x = 2.0;
        REQUIRE(tr.GetTraceHeaderKeyValue<float>(TraceHeaderKey::SCALCO) == x);
    }
}


/**
 * REQUIRED TESTS:
 *
 * 1. All functions individually to return desired output in same test case yet different sections.
 * 2. Data pointers to be tested that they are unique pointers.
 * 3. Copy constructor to copy correctly.
 * 4. Move constructor to move correctly.
 * 5. Logical operators to compare correctly.
 */

TEST_CASE("TraceTest", "[Trace]") {
    TEST_TRACE();

}
