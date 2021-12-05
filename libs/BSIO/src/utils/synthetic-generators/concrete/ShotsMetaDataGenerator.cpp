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

#include <bs/io/utils/synthetic-generators/concrete/ShotsMetaDataGenerator.hpp>

#define IO_SPACE_SCALE_FACTOR    1e3
#define IO_TIME_SCALE_FACTOR     1e6f

using namespace std;
using namespace bs::io::generators;
using namespace bs::io::dataunits;


ShotsMetaDataGenerator::~ShotsMetaDataGenerator() = default;

ShotsMetaDataGenerator::ShotsMetaDataGenerator(nlohmann::json aJsonNode)
        : mGatherKey(TraceHeaderKey::NS) {
    /// @todo: add better error handling-parsing.
    this->mSamplingX = aJsonNode[IO_K_SYN_CELL_SAMP][IO_K_SYN_CELL_SAMP_X];
    this->mSamplingY = aJsonNode[IO_K_SYN_CELL_SAMP][IO_K_SYN_CELL_SAMP_Y];
    this->mTimeSampling = aJsonNode[IO_K_SYN_DT];
    this->mSampleNumber = aJsonNode[IO_K_SYN_NS];
    this->mSourceStart.x = aJsonNode[IO_K_SYN_SOURCE][IO_K_SYN_START]
    [IO_K_SYN_X_INDEX];
    this->mSourceStart.y = aJsonNode[IO_K_SYN_SOURCE][IO_K_SYN_START]
    [IO_K_SYN_Y_INDEX];
    this->mSourceStart.z = aJsonNode[IO_K_SYN_SOURCE][IO_K_SYN_START]
    [IO_K_SYN_Z_INDEX];
    this->mSourceEnd.x = aJsonNode[IO_K_SYN_SOURCE][IO_K_SYN_END]
    [IO_K_SYN_X_INDEX];
    this->mSourceEnd.y = aJsonNode[IO_K_SYN_SOURCE][IO_K_SYN_END]
    [IO_K_SYN_Y_INDEX];
    this->mSourceEnd.z = aJsonNode[IO_K_SYN_SOURCE][IO_K_SYN_END]
    [IO_K_SYN_Z_INDEX];
    this->mSourceIncrement.x = aJsonNode[IO_K_SYN_SOURCE]
    [IO_K_SYN_INCREMENT][IO_K_SYN_X_INDEX];
    this->mSourceIncrement.y = aJsonNode[IO_K_SYN_SOURCE]
    [IO_K_SYN_INCREMENT][IO_K_SYN_Y_INDEX];
    this->mSourceIncrement.z = aJsonNode[IO_K_SYN_SOURCE]
    [IO_K_SYN_INCREMENT][IO_K_SYN_Z_INDEX];
    this->mReceiverRelStart.x = aJsonNode[IO_K_SYN_REC_REL]
    [IO_K_SYN_START][IO_K_SYN_X_OFFSET];
    this->mReceiverRelStart.y = aJsonNode[IO_K_SYN_REC_REL]
    [IO_K_SYN_START][IO_K_SYN_Y_OFFSET];
    this->mReceiverRelStart.z = aJsonNode[IO_K_SYN_REC_REL]
    [IO_K_SYN_START][IO_K_SYN_Z_OFFSET];
    this->mReceiverNumber.x = aJsonNode[IO_K_SYN_REC_REL]
    [IO_K_SYN_NUMBER][IO_K_SYN_X];
    this->mReceiverNumber.y = aJsonNode[IO_K_SYN_REC_REL]
    [IO_K_SYN_NUMBER][IO_K_SYN_Y];
    this->mReceiverNumber.z = aJsonNode[IO_K_SYN_REC_REL]
    [IO_K_SYN_NUMBER][IO_K_SYN_Z];
    this->mReceiverIncrement.x = aJsonNode[IO_K_SYN_REC_REL]
    [IO_K_SYN_INCREMENT][IO_K_SYN_X_INDEX];
    this->mReceiverIncrement.y = aJsonNode[IO_K_SYN_REC_REL]
    [IO_K_SYN_INCREMENT][IO_K_SYN_Y_INDEX];
    this->mReceiverIncrement.z = aJsonNode[IO_K_SYN_REC_REL]
    [IO_K_SYN_INCREMENT][IO_K_SYN_Z_INDEX];
    this->mOriginX = aJsonNode[IO_K_SYN_ORIGIN][IO_K_SYN_X];
    this->mOriginY = aJsonNode[IO_K_SYN_ORIGIN][IO_K_SYN_Y];
    this->mReceiverNumber.x = this->mReceiverNumber.x == 0 ? 1 : this->mReceiverNumber.x;
    this->mReceiverNumber.y = this->mReceiverNumber.y == 0 ? 1 : this->mReceiverNumber.y;
    this->mReceiverNumber.z = this->mReceiverNumber.z == 0 ? 1 : this->mReceiverNumber.z;
    this->mSourceIncrement.x = this->mSourceIncrement.x == 0 ? 1 : this->mSourceIncrement.x;
    this->mSourceIncrement.y = this->mSourceIncrement.y == 0 ? 1 : this->mSourceIncrement.y;
    this->mSourceIncrement.z = this->mSourceIncrement.z == 0 ? 1 : this->mSourceIncrement.z;
}

void
ShotsMetaDataGenerator::SetGenerationKey(vector<TraceHeaderKey> &aHeaderKeys) {
    if (aHeaderKeys.size() != 1) {
        throw bs::base::exceptions::UNSUPPORTED_FEATURE_EXCEPTION();
    }
    this->mGatherKey = aHeaderKeys[0];
    if (this->mGatherKey != TraceHeaderKey::FLDR) {
        throw bs::base::exceptions::UNSUPPORTED_FEATURE_EXCEPTION();
    }
}

vector<Gather *>
ShotsMetaDataGenerator::GetAllTraces() {
    std::vector<io::dataunits::Gather *> results;
    int inc_x = this->mSourceIncrement.x;
    int inc_y = this->mSourceIncrement.y;
    int shot_id = 1;
    for (int sy = this->mSourceStart.y; sy <= this->mSourceEnd.y; sy += inc_y) {
        for (int sx = this->mSourceStart.x; sx <= this->mSourceEnd.x; sx += inc_x) {
            auto gather = new Gather();
            for (int ry = 0; ry < this->mReceiverNumber.y; ry++) {
                for (int rx = 0; rx < this->mReceiverNumber.x; rx++) {
                    auto trace = new Trace(this->mSampleNumber);
                    int16_t factor = -IO_SPACE_SCALE_FACTOR;
                    trace->SetTraceHeaderKeyValue(TraceHeaderKey::SCALCO, factor);
                    float src_x = this->mOriginX + sx * this->mSamplingX;
                    float src_y = this->mOriginY + sy * this->mSamplingY;
                    float rec_x = this->mOriginX
                                  + (sx + this->mReceiverRelStart.x
                                     + rx * this->mReceiverIncrement.x) *
                                    this->mSamplingX;
                    float rec_y = this->mOriginY
                                  + (sy + this->mReceiverRelStart.y
                                     + ry * this->mReceiverIncrement.y) *
                                    this->mSamplingY;
                    trace->SetScaledCoordinateHeader(TraceHeaderKey::SX, src_x);
                    trace->SetScaledCoordinateHeader(TraceHeaderKey::SY, src_y);
                    trace->SetScaledCoordinateHeader(TraceHeaderKey::GX, rec_x);
                    trace->SetScaledCoordinateHeader(TraceHeaderKey::GY, rec_y);
                    uint16_t sampling = this->mTimeSampling * IO_TIME_SCALE_FACTOR;
                    trace->SetTraceHeaderKeyValue(TraceHeaderKey::DT, sampling);
                    trace->SetTraceHeaderKeyValue(TraceHeaderKey::SYN_X_IND, sx);
                    trace->SetTraceHeaderKeyValue(TraceHeaderKey::SYN_Y_IND, sy);
                    trace->SetTraceHeaderKeyValue(TraceHeaderKey::FLDR, shot_id);
                    trace->SetTraceData(new float[this->mSampleNumber]);
                    memset(trace->GetTraceData(), 0, this->mSampleNumber * sizeof(float));
                    gather->AddTrace(trace);
                }
            }
            gather->SetSamplingRate(this->mTimeSampling * IO_TIME_SCALE_FACTOR);
            std::string shot_id_string = std::to_string(shot_id);
            gather->SetUniqueKeyValue(this->mGatherKey,
                                      shot_id_string);
            shot_id++;
            results.push_back(gather);
        }
    }
    return results;
}

vector<Gather *>
ShotsMetaDataGenerator::GetTraces(vector<vector<string>> aHeaderValues) {
    std::vector<io::dataunits::Gather *> results;
    int number_of_source_x = abs(this->mSourceEnd.x - this->mSourceStart.x) + abs(this->mSourceIncrement.x);
    number_of_source_x = number_of_source_x / abs(this->mSourceIncrement.x);
    for (auto const &header_value : aHeaderValues) {
        int shot_number = stoi(header_value[0]);
        int shot_number_mod = shot_number - 1;
        int x_iter = shot_number_mod % number_of_source_x;
        int y_iter = shot_number_mod / number_of_source_x;
        int sx = this->mSourceStart.x + x_iter * this->mSourceIncrement.x;
        int sy = this->mSourceStart.y + y_iter * this->mSourceIncrement.y;
        auto gather = new Gather();
        for (int ry = 0; ry < this->mReceiverNumber.y; ry++) {
            for (int rx = 0; rx < this->mReceiverNumber.x; rx++) {
                auto trace = new Trace(this->mSampleNumber);
                int16_t factor = -IO_SPACE_SCALE_FACTOR;
                trace->SetTraceHeaderKeyValue(TraceHeaderKey::SCALCO, factor);
                float src_x = this->mOriginX + sx * this->mSamplingX;
                float src_y = this->mOriginY + sy * this->mSamplingY;
                float rec_x = this->mOriginX
                              + (sx + this->mReceiverRelStart.x
                                 + rx * this->mReceiverIncrement.x) *
                                this->mSamplingX;
                float rec_y = this->mOriginY
                              + (sy + this->mReceiverRelStart.y
                                 + ry * this->mReceiverIncrement.y) *
                                this->mSamplingY;
                trace->SetScaledCoordinateHeader(TraceHeaderKey::SX, src_x);
                trace->SetScaledCoordinateHeader(TraceHeaderKey::SY, src_y);
                trace->SetScaledCoordinateHeader(TraceHeaderKey::GX, rec_x);
                trace->SetScaledCoordinateHeader(TraceHeaderKey::GY, rec_y);
                uint16_t sampling = this->mTimeSampling * IO_TIME_SCALE_FACTOR;
                trace->SetTraceHeaderKeyValue(TraceHeaderKey::DT, sampling);
                trace->SetTraceHeaderKeyValue(TraceHeaderKey::SYN_X_IND, sx);
                trace->SetTraceHeaderKeyValue(TraceHeaderKey::SYN_Y_IND, sy);
                trace->SetTraceHeaderKeyValue(TraceHeaderKey::FLDR, shot_number);
                trace->SetTraceData(new float[this->mSampleNumber]);
                memset(trace->GetTraceData(), 0, this->mSampleNumber * sizeof(float));
                gather->AddTrace(trace);
            }
        }
        gather->SetSamplingRate(this->mTimeSampling * IO_TIME_SCALE_FACTOR);
        std::string shot_number_mod_string = std::to_string(shot_number_mod + 1);
        gather->SetUniqueKeyValue(this->mGatherKey,
                                  shot_number_mod_string);
        results.push_back(gather);
    }
    return results;
}

vector<std::vector<std::string>>
ShotsMetaDataGenerator::GetGatherUniqueValues() {
    uint source_number = GetSourceNumber();
    std::vector<std::vector<std::string>> results;
    results.reserve(source_number);
    for (uint i = 0; i < source_number; i++) {
        results.push_back(vector<string>{to_string(i + 1)});
    }
    return results;
}

uint
ShotsMetaDataGenerator::GetGatherNumber() {
    return GetSourceNumber() * GetReceiverNumber();
}

uint
ShotsMetaDataGenerator::GetSourceNumber() const {
    uint number_of_source_x = abs(this->mSourceEnd.x - this->mSourceStart.x) + abs(this->mSourceIncrement.x);
    number_of_source_x = number_of_source_x / abs(this->mSourceIncrement.x);
    uint number_of_source_y = abs(this->mSourceEnd.y - this->mSourceStart.y) + abs(this->mSourceIncrement.y);
    number_of_source_y = number_of_source_y / abs(this->mSourceIncrement.y);
    uint number_of_source_z = abs(this->mSourceEnd.z - this->mSourceStart.z) + abs(this->mSourceIncrement.z);
    number_of_source_z = number_of_source_z / abs(this->mSourceIncrement.z);
    uint number_of_sources = number_of_source_x * number_of_source_y * number_of_source_z;
    return number_of_sources;
}

uint
ShotsMetaDataGenerator::GetReceiverNumber() const {
    uint number_receiver_per_shot = 1;
    number_receiver_per_shot *= this->mReceiverNumber.x;
    number_receiver_per_shot *= this->mReceiverNumber.y;
    number_receiver_per_shot *= this->mReceiverNumber.z;
    return number_receiver_per_shot;
}
