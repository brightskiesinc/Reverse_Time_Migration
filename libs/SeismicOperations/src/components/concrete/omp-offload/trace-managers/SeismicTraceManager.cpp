//
// Created by ingy-mounir on 1/28/20.
//

#include "operations/components/independents/concrete/trace-managers/SeismicTraceManager.hpp"

#include <operations/utils/interpolation/Interpolator.hpp>
#include <operations/utils/io/read_utils.h>

#include <timer/Timer.h>
#include <memory-manager/MemoryManager.h>
#include <seismic-io-framework/datatypes.h>

#include <iostream>
#include <cmath>
#include <omp.h>

using namespace std;
using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;
using namespace operations::utils::io;


#pragma omp declare target

void ApplyOnPressure(float std_offset, int trace_offset, int wnz_wnx, float *pressure, float *vel, float *traces,
                     uint *position_y, uint *position_x, int i) {

    int offset = position_y[i] * wnz_wnx + std_offset + position_x[i];

    pressure[offset] += traces[trace_offset] * vel[offset];

}

#pragma omp end declare target


void SeismicTraceManager::ApplyTraces(uint time_step) {
    int trace_size = this->mpTracesHolder->TraceSizePerTimeStep;
    int wnx = this->mpGridBox->GetActualWindowSize(X_AXIS);
    int wnz_wnx = this->mpGridBox->GetActualWindowSize(Z_AXIS) * wnx;
    int std_offset = (mpParameters->GetBoundaryLength() + mpParameters->GetHalfLength()) * wnx;
    float current_time = (time_step - 1) * this->mpGridBox->GetDT();
    float dt = this->mpGridBox->GetDT();

    uint trace_step = uint(current_time / this->mpTracesHolder->SampleDT);
    trace_step = min(trace_step, this->mpTracesHolder->SampleNT - 1);

    float *pressure = this->mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();
    float *velocity = this->mpGridBox->Get(PARM | WIND | GB_VEL)->GetNativePointer();

    float *traces = this->mpDTraces.GetNativePointer();
    uint *pos_x = this->mpDPositionsX.GetNativePointer();
    uint *pos_y = this->mpDPositionsY.GetNativePointer();

    int device_num = omp_get_default_device();

#pragma omp target is_device_ptr(pressure, velocity, traces, pos_y, pos_x) device(device_num)
#pragma omp parallel for
    for (int i = 0; i < trace_size; i++) {

        int trace_offset = (trace_step) * trace_size + i;

        ApplyOnPressure(std_offset, trace_offset, wnz_wnx, pressure, velocity, traces, pos_y, pos_x, i);

/*
        if (this->mpParameters->GetApproximation() == VTI ||
            this->mpParameters->GetApproximation() == TTI) {
        
        	float *horizontal_pressure = this->mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_X)->GetNativePointer();
            ApplyOnPressure(std_offset, trace_offset,wnz_wnx  ,horizontal_pressure, velocity,traces,pos_y,pos_x,i);

        }
  */
    }
}
