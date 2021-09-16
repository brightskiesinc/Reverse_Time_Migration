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

#include "operations/components/independents/concrete/trace-managers/SeismicTraceManager.hpp"
#include <bs/base/memory/MemoryManager.hpp>


using namespace std;
using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;
using namespace bs::base::memory;


void SeismicTraceManager::ApplyTraces(int time_step) {
    int trace_size = this->mpTracesHolder->TraceSizePerTimeStep;

    int wnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    int wnz_wnx = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize() * wnx;

    int std_offset = (mpParameters->GetBoundaryLength() + mpParameters->GetHalfLength()) * wnx;
    float current_time = (time_step - 1) * this->mpGridBox->GetDT();
    float dt = this->mpGridBox->GetDT();

    uint trace_step = uint(current_time / this->mpTracesHolder->SampleDT);
    trace_step = min(trace_step, this->mpTracesHolder->SampleNT - 1);
    auto traces = this->mpTracesHolder->Traces->GetNativePointer();
    for (int i = 0; i < trace_size; i++) {
        int offset = this->mpTracesHolder->PositionsY[i] * wnz_wnx + std_offset + this->mpTracesHolder->PositionsX[i];
        float *pressure = this->mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();
        float *velocity = this->mpGridBox->Get(PARM | WIND | GB_VEL)->GetNativePointer();
        pressure[offset] +=
                traces[(trace_step) * trace_size + i] *
                velocity[offset];

        if (this->mpParameters->GetApproximation() == VTI ||
            this->mpParameters->GetApproximation() == TTI) {
            float *horizontal_pressure = this->mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_X)->GetNativePointer();
            horizontal_pressure[offset] +=
                    traces[(trace_step) * trace_size + i] *
                    velocity[offset];
        }
    }
}
