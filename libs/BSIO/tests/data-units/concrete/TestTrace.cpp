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

#include <iostream>

#include <prerequisites/libraries/catch/catch.hpp>

#include <bs/io/data-units/concrete/Trace.hpp>

using namespace bs::io::dataunits;


void
TEST_TRACE() {
    SECTION("Constructor") {
        Trace t(10);
        REQUIRE(t.GetTraceHeaderKeyValue<int>(TraceHeaderKey(TraceHeaderKey::Key::NS)) == 10);
    }

    SECTION("Move Constructor") {
        Trace t(3);

        auto ground_truth_data = (float *) malloc(3 * sizeof(float));
        ground_truth_data[0] = 1.0;
        ground_truth_data[1] = 2.3;
        ground_truth_data[2] = 5.6;

        t.SetTraceData(ground_truth_data);
        Trace temp = std::move(t);
        float *test_data = temp.GetTraceData();

        REQUIRE(test_data[0] == ground_truth_data[0]);
        REQUIRE(test_data[1] == ground_truth_data[1]);
        REQUIRE(test_data[2] == ground_truth_data[2]);
    }

    SECTION("HasHeader") {
        int ns = 3;
        Trace t(ns);
        REQUIRE(t.HasTraceHeader(TraceHeaderKey::NS));
        /* FLDR is not set. */
        REQUIRE(!t.HasTraceHeader(TraceHeaderKey::FLDR));
        REQUIRE(t.GetNumberOfSamples() == ns);
        /* Only NS header is set. */
        REQUIRE(t.GetTraceHeaders()->size() == 1);

        int fldr = 1;
        t.SetTraceHeaderKeyValue(TraceHeaderKey::FLDR, fldr);
        /* FLDR is now set. */
        REQUIRE(t.HasTraceHeader(TraceHeaderKey::FLDR));
        /* FLDR and NS header are now set. */
        REQUIRE(t.GetTraceHeaders()->size() == 2);
    }

    SECTION("Set_Scaled_Coordinate") {
        Trace t(3);
        float ground_truth_location = 2.0f;
        t.SetScaledCoordinateHeader(TraceHeaderKey(TraceHeaderKey::SCALCO), ground_truth_location);
        REQUIRE(t.GetTraceHeaderKeyValue<float>(TraceHeaderKey::SCALCO) == ground_truth_location);
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
