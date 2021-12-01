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

#include <bs/base/api/cpp/BSBase.hpp>

#include <operations/components/independents/concrete/trace-managers/SeismicTraceManager.hpp>

using namespace std;
using namespace cl::sycl;
using namespace bs::base::backend;
using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;

void SeismicTraceManager::ApplyTraces(int time_step) {
    int trace_size = mpTracesHolder->TraceSizePerTimeStep;

    int wnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    int wnz_wnx = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize() * wnx;

    int std_offset = (mpParameters->GetBoundaryLength() + mpParameters->GetHalfLength()) * wnx;
    float current_time = (time_step - 1) * mpGridBox->GetDT();
    float dt = mpGridBox->GetDT();

    uint trace_step = uint(current_time / mpTracesHolder->SampleDT);
    trace_step = std::min(trace_step, mpTracesHolder->SampleNT - 1);

    Backend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
        auto global_range = range<1>(trace_size);
        auto local_range = range<1>(1);
        auto global_nd_range = nd_range<1>(global_range, local_range);

        float *current = mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();
        float *trace_values = mpDTraces.GetNativePointer();
        float *w_vel = mpGridBox->Get(PARM | WIND | GB_VEL)->GetNativePointer();
        uint *x_pos = mpDPositionsX.GetNativePointer();
        uint *y_pos = mpDPositionsY.GetNativePointer();
        cgh.parallel_for(global_nd_range, [=](nd_item<1> it) {
            int i = it.get_global_id(0);
            int offset = y_pos[i] * wnz_wnx + std_offset + x_pos[i];
            current[offset] += trace_values[(trace_step) * trace_size + i] * w_vel[offset];
        });
    });
    Backend::GetInstance()->GetDeviceQueue()->wait();
}
