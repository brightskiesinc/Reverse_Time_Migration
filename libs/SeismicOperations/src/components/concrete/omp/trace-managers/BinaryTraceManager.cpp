//
// Created by amr-nasr on 13/11/2019.
//

#include <operations/components/independents/concrete/trace-managers/BinaryTraceManager.hpp>

#include <operations/utils/interpolation/Interpolator.hpp>

#include <cmath>

using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;

void BinaryTraceManager::ApplyTraces(uint time_step) {
    int x_inc = this->mReceiverIncrement.x == 0 ? 1 : this->mReceiverIncrement.x;
    int y_inc = this->mReceiverIncrement.y == 0 ? 1 : this->mReceiverIncrement.y;
    int z_inc = this->mReceiverIncrement.z == 0 ? 1 : this->mReceiverIncrement.z;
    int trace_size = this->mpTracesHolder->TraceSizePerTimeStep;
    int wnx = this->mpGridBox->GetActualWindowSize(X_AXIS);
    int wnz_wnx = this->mpGridBox->GetActualWindowSize(Z_AXIS) * wnx;
    int index = 0;
    float current_time = (time_step - 1) * this->mpGridBox->GetDT();
    float dt = this->mpGridBox->GetDT();

    float *pressure = this->mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();
    uint trace_step = uint(current_time / this->mpTracesHolder->SampleDT);
    trace_step = std::min(trace_step, this->mpTracesHolder->SampleNT - 1);
    for (int iz = this->mReceiverStart.z; iz < this->mReceiverEnd.z; iz += z_inc) {
        for (int iy = this->mReceiverStart.y; iy < this->mReceiverEnd.y; iy += y_inc) {
            for (int ix = this->mReceiverStart.x; ix < this->mReceiverEnd.x; ix += x_inc) {
                int offset = iy * wnz_wnx + iz * wnx + ix;
                pressure[offset] +=
                        this->mpTracesHolder->Traces[(trace_step) * trace_size + index] *
                        this->mpGridBox->Get(PARM | WIND | GB_VEL)->GetNativePointer()[offset];
                index++;
            }
        }
    }
}