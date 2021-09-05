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

#include <bs/io/test-utils/DataGenerator.hpp>

using namespace std;
using namespace bs::io::testutils;
using namespace bs::io::dataunits;


DataGenerator::DataGenerator() = default;

Trace *
DataGenerator::GenerateTrace(uint16_t aNS, int32_t aFLDR) {
    auto trace = new Trace(aNS);

    /* Data generation. */
    auto trace_data = new float[aNS];
    for (int i = 0; i < aNS; ++i) {
        trace_data[i] = (float) rand() * 100 / RAND_MAX;
    }
    trace->SetTraceData((float *) trace_data);

    /* Trace header keys generation. */
    trace->SetTraceHeaderKeyValue(TraceHeaderKey::NS, aNS);
    trace->SetTraceHeaderKeyValue(TraceHeaderKey::FLDR, aFLDR);

    return trace;
}

vector<Trace *> DataGenerator::GenerateTraceVector(uint16_t aNS, int aCount, int32_t aStartFLDR) {
    std::vector<Trace *> vec;
    vec.reserve(aCount);
    auto fldr = aStartFLDR;
    for (int i = 0; i < aCount; ++i) {
        vec.push_back(DataGenerator::GenerateTrace(aNS, fldr++));
    }
    return vec;
}

vector<Trace *> DataGenerator::GenerateTraceVector(uint16_t aNS, const std::vector<int32_t> &aFLDR) {
    std::vector<Trace *> vec;
    vec.reserve(aFLDR.size());
    for (const auto &fldr : aFLDR) {
        vec.push_back(DataGenerator::GenerateTrace(aNS, fldr));
    }
    return vec;
}

Gather *
DataGenerator::GenerateGather(uint16_t aNS, int aCount, int32_t aStartFLDR) {
    auto g = new Gather();
    g->AddTrace(DataGenerator::GenerateTraceVector(aNS, aCount, aStartFLDR));
    return g;
}
