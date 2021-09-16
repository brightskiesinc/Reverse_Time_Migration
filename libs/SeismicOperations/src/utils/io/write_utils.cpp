/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of SeismicToolbox.
 *
 * SeismicToolbox is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SeismicToolbox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */

#include <operations/utils/io/write_utils.h>

#include <string>

using namespace std;
using namespace operations::utils::io;
using namespace bs::io::dataunits;


vector<Gather *>
operations::utils::io::TransformToGather(const float *apData, uint aNX, uint aNY, uint aNS,
                                         float aDX, float aDY, float aDS, float aSX, float aSY,
                                         int aOffsetX, int aOffsetY, uint aShots, float aSpaceScale,
                                         float aSampleScale) {
    std::vector<Gather *> results;
    uint full_cube_size = aNX * aNY * aNS;
    for (int shot = 0; shot < aShots; shot++) {
        auto gather = new Gather();
        for (int iy = 0; iy < aNY; iy++) {
            for (int ix = 0; ix < aNX; ix++) {
                auto trace = new Trace(aNS);
                int16_t factor = -aSpaceScale;
                trace->SetTraceHeaderKeyValue(TraceHeaderKey::SCALCO, factor);
                float loc_x = aSX + (ix + aOffsetX) * aDX;
                float loc_y = aSY + (iy + aOffsetY) * aDY;
                trace->SetScaledCoordinateHeader(TraceHeaderKey::SX, loc_x);
                trace->SetScaledCoordinateHeader(TraceHeaderKey::SY, loc_y);
                trace->SetScaledCoordinateHeader(TraceHeaderKey::GX, loc_x);
                trace->SetScaledCoordinateHeader(TraceHeaderKey::GY, loc_y);
                uint16_t sampling = aDS * aSampleScale;
                trace->SetTraceHeaderKeyValue(TraceHeaderKey::DT, sampling);
                trace->SetTraceHeaderKeyValue(TraceHeaderKey::FLDR, shot + 1);
                trace->SetTraceData(new float[aNS]);
                for (int is = 0; is < aNS; is++) {
                    trace->GetTraceData()[is] = apData[shot * full_cube_size + iy * aNX * aNS + is * aNX + ix];
                }
                gather->AddTrace(trace);
            }
        }
        gather->SetSamplingRate(aDS * aSampleScale);
        string val = to_string(shot + 1);
        gather->SetUniqueKeyValue(TraceHeaderKey::FLDR, val);
        results.push_back(gather);
    }
    return results;
}

vector<Gather *>
operations::utils::io::TransformToGather(const float *apData, uint aNX, uint aNY, uint aNS,
                                         float aDS,
                                         Gather *apMetaDataGather, uint aShots,
                                         float aSampleScale) {
    std::vector<Gather *> results;
    uint full_cube_size = aNX * aNY * aNS;
    for (int shot = 0; shot < aShots; shot++) {
        auto gather = new Gather();
        for (int iy = 0; iy < aNY; iy++) {
            for (int ix = 0; ix < aNX; ix++) {
                auto trace = new Trace(aNS);
                auto header_map = apMetaDataGather->GetTrace(iy * aNX + ix)->GetTraceHeaders();
                auto new_headers_map = ((std::unordered_map<TraceHeaderKey, std::string> *) trace->GetTraceHeaders());
                new_headers_map->insert(header_map->begin(), header_map->end());
                uint16_t sampling = aDS * aSampleScale;
                trace->SetTraceHeaderKeyValue(TraceHeaderKey::DT, sampling);
                trace->SetTraceHeaderKeyValue(TraceHeaderKey::FLDR, shot + 1);
                trace->SetTraceData(new float[aNS]);
                for (int is = 0; is < aNS; is++) {
                    trace->GetTraceData()[is] = apData[shot * full_cube_size + iy * aNX * aNS + is * aNX + ix];
                }
                gather->AddTrace(trace);
            }
        }
        gather->SetSamplingRate(aDS * aSampleScale);
        string val = to_string(shot + 1);
        gather->SetUniqueKeyValue(TraceHeaderKey::FLDR, val);
        results.push_back(gather);
    }
    return results;
}