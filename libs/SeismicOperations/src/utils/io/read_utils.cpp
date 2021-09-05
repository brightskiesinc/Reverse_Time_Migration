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

#include <operations/utils/io/read_utils.h>

#include <bs/base/memory/MemoryManager.hpp>

#include <cmath>
#include <unordered_set>
#include <algorithm>

using namespace std;
using namespace operations::utils::io;
using namespace operations::dataunits;
using namespace operations::common;
using namespace bs::base::memory;
using namespace bs::io::dataunits;

Gather *operations::utils::io::CombineGather(std::vector<Gather *> &aGatherVector) {
    int gather_count = aGatherVector.size();
    for (int gather_index = 1; gather_index < gather_count; gather_index++) {
        int trace_count = aGatherVector[gather_index]->GetNumberTraces();
        for (int trace_index = 0; trace_index < trace_count; trace_index++) {
            aGatherVector[0]->AddTrace(aGatherVector[gather_index]->GetTrace(trace_index));
        }
        delete aGatherVector[gather_index];
    }
    auto gather = aGatherVector[0];
    aGatherVector.clear();
    aGatherVector.push_back(gather);
    return gather;
}

void operations::utils::io::RemoveDuplicatesFromGather(Gather *apGather) {
    int size = apGather->GetNumberTraces();
    unordered_map<float, unordered_set<float>> unique_positions;
    for (int i = 0; i < size; i++) {
        auto cur_sx = apGather->GetTrace(i)->GetScaledCoordinateHeader(TraceHeaderKey::SX);
        auto cur_sy = apGather->GetTrace(i)->GetScaledCoordinateHeader(TraceHeaderKey::SY);
        if (unique_positions.find(cur_sx) == unique_positions.end()) {
            unique_positions[cur_sx] = {};
            unique_positions[cur_sx].emplace(cur_sy);
        } else {
            if (unique_positions[cur_sx].find(cur_sy) == unique_positions[cur_sx].end()) {
                unique_positions[cur_sx].emplace(cur_sy);
            } else {
                apGather->RemoveTrace(i);
                i--;
                size--;
            }
        }
    }
}

bool operations::utils::io::IsLineGather(Gather *apGather) {
    int count = apGather->GetNumberTraces();
    bool return_value = true;
    float tolerance = 0.1; /// 10% tolerance
    float initial_y_diff = (apGather->GetTrace(0)->GetScaledCoordinateHeader(TraceHeaderKey::SY) -
                            apGather->GetTrace(1)->GetScaledCoordinateHeader(TraceHeaderKey::SY));
    float initial_x_diff = (apGather->GetTrace(0)->GetScaledCoordinateHeader(TraceHeaderKey::SX) -
                            apGather->GetTrace(1)->GetScaledCoordinateHeader(TraceHeaderKey::SX));
    float initial_slope = 0;
    if (initial_x_diff != 0 && initial_y_diff != 0) {
        initial_slope = initial_y_diff / initial_x_diff;
    }

    for (int i = 2; i < count; i++) {
        float y_diff = (apGather->GetTrace(0)->GetScaledCoordinateHeader(TraceHeaderKey::SY) -
                        apGather->GetTrace(i)->GetScaledCoordinateHeader(TraceHeaderKey::SY));
        float x_diff = (apGather->GetTrace(0)->GetScaledCoordinateHeader(TraceHeaderKey::SX) -
                        apGather->GetTrace(i)->GetScaledCoordinateHeader(TraceHeaderKey::SX));
        if (initial_x_diff != 0 && initial_y_diff != 0) {
            if (x_diff == 0) {
                return_value = false;
                break;
            }
            float slope = y_diff / x_diff;
            if (!(fabsf(slope - initial_slope) <=
                  tolerance * (fabs(slope) < fabs(initial_slope) ? fabs(initial_slope) : fabs(slope)))) {
                return_value = false;
                break;
            }
        } else {
            if ((initial_x_diff == 0 && x_diff != 0) || (initial_y_diff == 0 && y_diff != 0)) {
                return_value = false;
                break;
            }
        }
    }
    return return_value;
}


void operations::utils::io::ParseGatherToTraces(
        bs::io::dataunits::Gather *apGather, Point3D *apSource, TracesHolder *apTraces,
        uint **x_position, uint **y_position,
        GridBox *apGridBox, ComputationParameters *apParameters,
        float *total_time) {
    uint ny = apGridBox->GetAfterSamplingAxis()->GetYAxis().GetActualAxisSize();
    std::unordered_set<uint> x_dim;
    std::unordered_set<uint> y_dim;
    // No need to sort as we deal with each trace with its position independently.
    // Get source point.
    float source_org_x;
    float source_org_y;
    float source_org_z;
    uint offset = apParameters->GetHalfLength() + apParameters->GetBoundaryLength();
    uint window_left_size = apGridBox->GetAfterSamplingAxis()->GetXAxis().GetLogicalAxisSize() - 2 * offset -
                            apParameters->GetRightWindow();
    uint window_back_size = apGridBox->GetAfterSamplingAxis()->GetYAxis().GetLogicalAxisSize() - 2 * offset -
                            apParameters->GetFrontWindow();
    source_org_x = apGather->GetTrace(0)->GetScaledCoordinateHeader(TraceHeaderKey::SX)
                   - apGridBox->GetAfterSamplingAxis()->GetXAxis().GetReferencePoint();
    source_org_z = 0;
    source_org_y = apGather->GetTrace(0)->GetScaledCoordinateHeader(TraceHeaderKey::SY)
                   - apGridBox->GetAfterSamplingAxis()->GetYAxis().GetReferencePoint();

    if (ny == 1) {
        source_org_x = sqrtf(source_org_x * source_org_x + source_org_y * source_org_y);
        source_org_y = 0;
    }
    apSource->x = round(source_org_x / apGridBox->GetAfterSamplingAxis()->GetXAxis().GetCellDimension());
    apSource->z = round(source_org_z / apGridBox->GetAfterSamplingAxis()->GetZAxis().GetCellDimension());
    apSource->y = round(source_org_y / apGridBox->GetAfterSamplingAxis()->GetYAxis().GetCellDimension());
    // If window model, need to setup the starting point of the window and adjust source point.
    // Handle 3 cases : no room for left window, no room for right window, room for both.
    // Those 3 cases can apply to y-direction as well if 3D.
    if (apParameters->IsUsingWindow()) {
        apGridBox->SetWindowStart(X_AXIS, 0);
        // No room for left window.
        if (apSource->x < apParameters->GetLeftWindow() ||
            (apParameters->GetLeftWindow() == 0 && apParameters->GetRightWindow() == 0)) {
            apGridBox->SetWindowStart(X_AXIS, 0);
            // No room for right window.
        } else if (apSource->x >= window_left_size) {
            uint window_start = window_left_size - 1 - apParameters->GetLeftWindow();
            apGridBox->SetWindowStart(X_AXIS, window_start);
            apSource->x = apSource->x - window_start;
        } else {
            apGridBox->SetWindowStart(X_AXIS, ((int) apSource->x) - apParameters->GetLeftWindow());
            apSource->x = apParameters->GetLeftWindow();
        }
        apGridBox->SetWindowStart(Y_AXIS, 0);
        if (ny != 1) {
            if (apSource->y < apParameters->GetBackWindow() ||
                (apParameters->GetFrontWindow() == 0 && apParameters->GetBackWindow() == 0)) {
                apGridBox->SetWindowStart(Y_AXIS, 0);
            } else if (apSource->y >= window_back_size) {
                uint window_start = window_back_size - 1 - apParameters->GetBackWindow();
                apGridBox->SetWindowStart(Y_AXIS, window_start);
                apSource->y = apSource->y - window_start;
            } else {
                apGridBox->SetWindowStart(Y_AXIS, ((int) apSource->y) - apParameters->GetBackWindow());
                apSource->y = apParameters->GetBackWindow();
            }
        }
    }
    apSource->x += offset;
    apSource->z += offset;
    if (ny > 1) {
        apSource->y += offset;
    }
    // Begin traces parsing.
    // Remove traces outside the window.
    uint intern_x = apGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize() - 2 * offset;
    uint intern_y = apGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize() - 2 * offset;
    for (int i = ((int) apGather->GetNumberTraces()) - 1; i >= 0; i--) {
        bool erased = false;
        float gx_loc = apGather->GetTrace(i)->GetScaledCoordinateHeader(TraceHeaderKey::GX)
                       - apGridBox->GetAfterSamplingAxis()->GetXAxis().GetReferencePoint();
        float gy_loc = apGather->GetTrace(i)->GetScaledCoordinateHeader(TraceHeaderKey::GY)
                       - apGridBox->GetAfterSamplingAxis()->GetYAxis().GetReferencePoint();

        if (ny == 1) {
            gx_loc = sqrtf(gx_loc * gx_loc + gy_loc * gy_loc);
            gy_loc = 0;
        }
        uint gx = round(gx_loc / apGridBox->GetAfterSamplingAxis()->GetXAxis().GetCellDimension());
        uint gy = round(gy_loc / apGridBox->GetAfterSamplingAxis()->GetYAxis().GetCellDimension());
        if (gx < apGridBox->GetWindowStart(X_AXIS) || gx >= apGridBox->GetWindowStart(X_AXIS) + intern_x) {
            apGather->RemoveTrace(i);
            erased = true;
        } else if (apGridBox->GetAfterSamplingAxis()->GetYAxis().GetLogicalAxisSize() != 1) {
            if (gy < apGridBox->GetWindowStart(Y_AXIS) || gy >= apGridBox->GetWindowStart(Y_AXIS) + intern_y) {
                apGather->RemoveTrace(i);
                erased = true;
            }
        }

        if (!erased) {
            x_dim.insert(gx_loc);
            y_dim.insert(gy_loc);
        }
    }
    /* Set meta data. */
    apTraces->SampleDT = apGather->GetSamplingRate() / (float) 1e6;
    int sample_nt = apGather->GetTrace(0)->GetNumberOfSamples();

    int num_elements_per_time_step = apGather->GetNumberTraces();
    apTraces->TraceSizePerTimeStep = apGather->GetNumberTraces();
    apTraces->ReceiversCountX = x_dim.size();
    apTraces->ReceiversCountY = y_dim.size();
    apTraces->SampleNT = sample_nt;

    /// We dont have total time , but we have the nt from the
    /// segy file , so we can modify the nt according to the
    /// ratio between the recorded dt and the suitable dt
    apGridBox->SetNT(int(sample_nt * apTraces->SampleDT / apGridBox->GetDT()));

    *total_time = sample_nt * apTraces->SampleDT;

    *x_position = (uint *) mem_allocate(
            sizeof(uint), num_elements_per_time_step, "traces x-position");
    *y_position = (uint *) mem_allocate(
            sizeof(uint), num_elements_per_time_step, "traces y-position");

    auto traces = (float *) mem_allocate(sizeof(float), sample_nt * num_elements_per_time_step, "traces_tmp");
    for (int trace_index = 0; trace_index < num_elements_per_time_step; trace_index++) {
        for (int t = 0; t < sample_nt; t++) {
            traces[t * num_elements_per_time_step + trace_index]
                    = apGather->GetTrace(trace_index)->GetTraceData()[t];
        }
        float gx_loc = apGather->GetTrace(trace_index)->GetScaledCoordinateHeader(TraceHeaderKey::GX)
                       - apGridBox->GetAfterSamplingAxis()->GetXAxis().GetReferencePoint();
        float gy_loc = apGather->GetTrace(trace_index)->GetScaledCoordinateHeader(TraceHeaderKey::GY)
                       - apGridBox->GetAfterSamplingAxis()->GetYAxis().GetReferencePoint();
        if (ny == 1) {
            gx_loc = sqrtf(gx_loc * gx_loc + gy_loc * gy_loc);
            gy_loc = 0;
        }
        uint gx = round(gx_loc / apGridBox->GetAfterSamplingAxis()->GetXAxis().GetCellDimension());
        uint gy = round(gy_loc / apGridBox->GetAfterSamplingAxis()->GetYAxis().GetCellDimension());
        gx -= apGridBox->GetWindowStart(X_AXIS);
        gy -= apGridBox->GetWindowStart(Y_AXIS);
        (*x_position)[trace_index] = gx + offset;
        if (apGridBox->GetAfterSamplingAxis()->GetYAxis().GetLogicalAxisSize() > 1) {
            (*y_position)[trace_index] = gy + offset;
        } else {
            (*y_position)[trace_index] = gy;
        }
    }
    /* Setup traces data to the arrays. */
    apTraces->Traces = new FrameBuffer<float>;
    apTraces->Traces->Allocate(sample_nt * num_elements_per_time_step, "traces");
    Device::MemCpy(apTraces->Traces->GetNativePointer(), traces,
                   sample_nt * num_elements_per_time_step * sizeof(float),
                   Device::COPY_HOST_TO_DEVICE);
    mem_free(traces);
}
