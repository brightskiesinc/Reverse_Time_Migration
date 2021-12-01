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

#include <bs/base/exceptions/Exceptions.hpp>

#include <bs/io/utils/synthetic-generators/concrete/ParameterMetaDataGenerator.hpp>

#define SCALE_FACTOR    1e3

using namespace std;
using namespace bs::io::generators;
using namespace bs::io::dataunits;

ParameterMetaDataGenerator::ParameterMetaDataGenerator(nlohmann::json aJsonNode)
        : mGatherKey(TraceHeaderKey::NS) {
    //TODO add better error handling-parsing.
    this->mSamplingX = aJsonNode[IO_K_SYN_CELL_SAMP][IO_K_SYN_CELL_SAMP_X];
    this->mSamplingY = aJsonNode[IO_K_SYN_CELL_SAMP][IO_K_SYN_CELL_SAMP_Y];
    this->mSamplingZ = aJsonNode[IO_K_SYN_CELL_SAMP][IO_K_SYN_CELL_SAMP_Z];
    this->mOriginX = aJsonNode[IO_K_SYN_ORIGIN][IO_K_SYN_X];
    this->mOriginY = aJsonNode[IO_K_SYN_ORIGIN][IO_K_SYN_Y];
    this->mPointsX = aJsonNode[IO_K_SYN_GRIZ_SIZE][IO_K_SYN_NX];
    this->mPointsY = aJsonNode[IO_K_SYN_GRIZ_SIZE][IO_K_SYN_NY];
    this->mPointsZ = aJsonNode[IO_K_SYN_GRIZ_SIZE][IO_K_SYN_NZ];
}

void
ParameterMetaDataGenerator::SetGenerationKey(vector<TraceHeaderKey> &aHeaderKeys) {
    if (aHeaderKeys.size() > 1) {
        throw bs::base::exceptions::UNSUPPORTED_FEATURE_EXCEPTION();
    }
    if (aHeaderKeys.size() == 1) {
        this->mGatherKey = aHeaderKeys[0];
    } else {
        this->mGatherKey = TraceHeaderKey::FLDR;
    }
    if (this->mGatherKey != TraceHeaderKey::FLDR) {
        throw bs::base::exceptions::UNSUPPORTED_FEATURE_EXCEPTION();
    }
}

vector<Gather *>
ParameterMetaDataGenerator::GetAllTraces() {
    std::vector<io::dataunits::Gather *> results;
    auto gather = new Gather();
    for (int y = 0; y < this->mPointsY; y++) {
        for (int x = 0; x < this->mPointsX; x++) {
            auto trace = new Trace(this->mPointsZ);
            int16_t factor = -SCALE_FACTOR;
            trace->SetTraceHeaderKeyValue(TraceHeaderKey::SCALCO, factor);
            float loc_x = this->mOriginX + x * this->mSamplingX;
            float loc_y = this->mOriginY + y * this->mSamplingY;
            trace->SetScaledCoordinateHeader(TraceHeaderKey::SX, loc_x);
            trace->SetScaledCoordinateHeader(TraceHeaderKey::SY, loc_y);
            trace->SetScaledCoordinateHeader(TraceHeaderKey::GX, loc_x);
            trace->SetScaledCoordinateHeader(TraceHeaderKey::GY, loc_y);
            uint16_t sampling = this->mSamplingZ * SCALE_FACTOR;
            trace->SetTraceHeaderKeyValue(TraceHeaderKey::DT, sampling);
            trace->SetTraceHeaderKeyValue(TraceHeaderKey::SYN_X_IND, x);
            trace->SetTraceHeaderKeyValue(TraceHeaderKey::SYN_Y_IND, y);
            trace->SetTraceHeaderKeyValue(TraceHeaderKey::FLDR, 1);
            trace->SetTraceData(new float[this->mPointsZ]);
            memset(trace->GetTraceData(), 0, this->mPointsZ * sizeof(float));
            gather->AddTrace(trace);
        }
    }
    gather->SetSamplingRate(this->mSamplingZ * SCALE_FACTOR);
    std::string value = std::to_string(1);
    gather->SetUniqueKeyValue(this->mGatherKey, value);
    results.push_back(gather);
    return results;
}

vector<Gather *>
ParameterMetaDataGenerator::GetTraces(vector<vector<string>> aHeaderValues) {
    std::vector<io::dataunits::Gather *> results;
    if (aHeaderValues[0][0] == "1") {
        results = this->GetAllTraces();
    }
    return results;
}

vector<std::vector<std::string>>
ParameterMetaDataGenerator::GetGatherUniqueValues() {
    std::vector<std::vector<std::string>> results = {{"1"}};
    return results;
}

uint
ParameterMetaDataGenerator::GetGatherNumber() {
    return 1;
}
