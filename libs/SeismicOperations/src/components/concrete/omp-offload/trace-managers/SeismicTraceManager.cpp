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

#include <operations/components/independents/concrete/trace-managers/SeismicTraceManager.hpp>

#include <operations/utils/interpolation/Interpolator.hpp>
#include <operations/utils/io/read_utils.h>
#include <operations/utils/checks/Checks.hpp>

#include <omp.h>

using namespace std;
using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;
using namespace operations::utils::checks;
using namespace bs::base::exceptions;
using namespace operations::utils::io;


// Adding the traces and velocity values over the pressure array
#pragma omp declare target

void ApplyOnPressure(int aOffset, int aTraceOffset, float *aPressure, const float *aVelocity, const float *aTraces) {
    aPressure[aOffset] += aTraces[aTraceOffset] * aVelocity[aOffset];
}

#pragma omp end declare target

void SeismicTraceManager::ApplyTraces(int time_step) {

    int trace_size = this->mpTracesHolder->TraceSizePerTimeStep;

    int wnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    int wnz_wnx = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize() * wnx;

    int std_offset = (mpParameters->GetBoundaryLength() + mpParameters->GetHalfLength()) * wnx;
    float current_time = (time_step - 1) * this->mpGridBox->GetDT();

    uint trace_step = uint(current_time / this->mpTracesHolder->SampleDT);
    trace_step = min(trace_step, this->mpTracesHolder->SampleNT - 1);

    float *pressure = this->mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();
    float *velocity = this->mpGridBox->Get(PARM | WIND | GB_VEL)->GetNativePointer();

    float *traces = this->mpDTraces.GetNativePointer();
    uint *pos_x = this->mpDPositionsX.GetNativePointer();
    uint *pos_y = this->mpDPositionsY.GetNativePointer();

    int device_num = omp_get_default_device();

    if (is_device_not_exist()) {
        throw DEVICE_NOT_FOUND_EXCEPTION();
    }


#pragma omp target is_device_ptr(pressure, velocity, traces, pos_y, pos_x) device(device_num)
#pragma omp parallel for
    for (int i = 0; i < trace_size; i++) {

        int trace_offset = (trace_step) * trace_size + i;
        int offset = pos_y[i] * wnz_wnx + std_offset + pos_x[i];

        ApplyOnPressure(offset, trace_offset, pressure, velocity, traces);

// for future purpose , will be used after implementing vti and tti
/* 
        if (this->mpParameters->GetApproximation() == VTI ||
            this->mpParameters->GetApproximation() == TTI) {

            float *horizontal_pressure = this->mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_X)->GetNativePointer();
            ApplyOnPressure(std_offset, trace_offset,wnz_wnx  ,horizontal_pressure, velocity,traces,pos_y,pos_x,i);

        }
*/
    }
}
