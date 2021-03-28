//
// Created by ingy-mounir on 1/28/20.
//

#include "operations/components/independents/concrete/trace-managers/SeismicTraceManager.hpp"
#include <memory-manager/MemoryManager.h>


using namespace std;
using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;

void SeismicTraceManager::ApplyTraces(uint time_step) {
    int trace_size = this->mpTracesHolder->TraceSizePerTimeStep;
    int wnx = this->mpGridBox->GetActualWindowSize(X_AXIS);
    int wnz_wnx = this->mpGridBox->GetActualWindowSize(Z_AXIS) * wnx;
    int std_offset = (mpParameters->GetBoundaryLength() + mpParameters->GetHalfLength()) * wnx;
    float current_time = (time_step - 1) * this->mpGridBox->GetDT();
    float dt = this->mpGridBox->GetDT();

    uint trace_step = uint(current_time / this->mpTracesHolder->SampleDT);
    trace_step = min(trace_step, this->mpTracesHolder->SampleNT - 1);

    for (int i = 0; i < trace_size; i++) {
        int offset = this->mpTracesHolder->PositionsY[i] * wnz_wnx + std_offset + this->mpTracesHolder->PositionsX[i];
        float *pressure = this->mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();
        float *velocity = this->mpGridBox->Get(PARM | WIND | GB_VEL)->GetNativePointer();
        pressure[offset] +=
                this->mpTracesHolder->Traces[(trace_step) * trace_size + i] *
                velocity[offset];

        if (this->mpParameters->GetApproximation() == VTI ||
            this->mpParameters->GetApproximation() == TTI) {
            float *horizontal_pressure = this->mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_X)->GetNativePointer();
            horizontal_pressure[offset] +=
                    this->mpTracesHolder->Traces[(trace_step) * trace_size + i] *
                    velocity[offset];
        }
    }
}
