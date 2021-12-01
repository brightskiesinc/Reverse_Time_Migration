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

#include <bs/io/data-units/helpers/TraceHelper.hpp>
#include <bs/io/utils/convertors/NumbersConvertor.hpp>
#include <bs/io/data-units/concrete/Trace.hpp>
#include <bs/io/test-utils/DataGenerator.hpp>


using namespace bs::io::lookups;
using namespace bs::io::dataunits;
using namespace bs::io::dataunits::helpers;
using namespace bs::io::testutils;
using namespace bs::io::utils::convertors;


void
TEST_TRACE_HELPER() {
    Trace *trace;
    TraceHeaderLookup thl{};
    BinaryHeaderLookup bhl{};

    SECTION("No Weighting Needed") {
        int16_t format = 1;
        int16_t trwf = 2;
        bhl.FORMAT = NumbersConvertor::ToLittleEndian(format);; /* Converted to 2 in Little Endian Conversion. */
        thl.TRWF = NumbersConvertor::ToLittleEndian(trwf);
        trace = DataGenerator::GenerateTrace(10, 1);
        float data[10];
        memcpy(data, trace->GetTraceData(), 10 * sizeof(float));
        TraceHelper::Weight(trace, thl, bhl);
        REQUIRE(memcmp(data, trace->GetTraceData(), 10 * sizeof(float)) == 0);
    }

    SECTION("Weighting") {
        int16_t format = 2;
        int16_t trwf = 2;
        bhl.FORMAT = NumbersConvertor::ToLittleEndian(format); /* Converted to 2 in Little Endian Conversion. */
        thl.TRWF = NumbersConvertor::ToLittleEndian(trwf);
        trace = DataGenerator::GenerateTrace(10, 1);
        float data[10];
        memcpy(data, trace->GetTraceData(), 10 * sizeof(float));
        TraceHelper::Weight(trace, thl, bhl);
        float *weighted = trace->GetTraceData();
        for (int i = 0; i < 10; i++) {
            REQUIRE(weighted[i] == data[i] * std::pow(2.0, -2));
        }
        REQUIRE(thl.TRWF == 0);
    }
}


/**
 * REQUIRED TESTS:
 *
 * 1. Dummy trace to be generated and then weighting to be applied upon
 *    it's data and headers and compare results to ground truth.
 */

TEST_CASE("TraceHelper") {
    TEST_TRACE_HELPER();
}
