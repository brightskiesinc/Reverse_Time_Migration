//
// Created by ingy-mounir 20/10/2020.
//

#include <operations/components/independents/concrete/trace-managers/BinaryTraceManager.hpp>

#include <operations/utils/interpolation/Interpolator.hpp>

#include <iostream>
#include <cmath>
#include <omp.h>

using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;


#define GpuErrchk(ans) { GpuAssert((ans), __FILE__, __LINE__); }

void BinaryTraceManager::ApplyTraces(uint time_step) {
    int x_inc = this->mReceiverIncrement.x == 0 ? 1 : this->mReceiverIncrement.x;
    int y_inc = this->mReceiverIncrement.y == 0 ? 1 : this->mReceiverIncrement.y;
    int z_inc = this->mReceiverIncrement.z == 0 ? 1 : this->mReceiverIncrement.z;
    int trace_size = this->mpTracesHolder->TraceSizePerTimeStep;
    int wnx = mpGridBox->GetActualWindowSize(X_AXIS);
    int wnz_wnx = mpGridBox->GetActualWindowSize(Z_AXIS) * wnx;
    int index = 0;
    float current_time = (time_step - 1) * this->mpGridBox->GetDT();
    float dt = this->mpGridBox->GetDT();

    int device_num = omp_get_default_device();

    uint trace_step = uint(current_time / this->mpTracesHolder->SampleDT);
    trace_step = std::min(trace_step, this->mpTracesHolder->SampleNT - 1);

    float *velocity = this->mpGridBox->Get(PARM | WIND | GB_VEL)->GetNativePointer();
    float *traces = this->mpTracesHolder->Traces;
    float *pressure = this->mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();

#pragma omp target is_device_ptr(velocity, traces, pressure) device(device_num)
#pragma omp teams distribute parallel for collapse(3) schedule(static, 1)
    for (int iz = this->mReceiverStart.z; iz < this->mReceiverEnd.z; iz += z_inc) {
        for (int iy = this->mReceiverStart.y; iy < this->mReceiverEnd.y; iy += y_inc) {
            for (int ix = this->mReceiverStart.x; ix < this->mReceiverEnd.x; ix += x_inc) {
                int offset = iy * wnz_wnx + iz * wnx + ix;
                pressure[offset] += traces[(trace_step) * trace_size + index] * velocity[offset];
                index++;
            }
        }
    }

}
