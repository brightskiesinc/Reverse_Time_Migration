#include "operations/components/dependents/concrete/memory-handlers/WaveFieldsMemoryHandler.hpp"

#include <cmath>
#include <omp.h>
#include <iostream>
#include <cstring>

using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;

void WaveFieldsMemoryHandler::FirstTouch(float *ptr, GridBox *apGridBox, bool enable_window) {
    int nx, ny, nz;
    int device_num = omp_get_default_device();
    if (enable_window) {
        nx = apGridBox->GetActualWindowSize(X_AXIS);
        ny = apGridBox->GetActualWindowSize(Y_AXIS);
        nz = apGridBox->GetActualWindowSize(Z_AXIS);
    } else {
        nx = apGridBox->GetActualGridSize(X_AXIS);
        ny = apGridBox->GetActualGridSize(Y_AXIS);
        nz = apGridBox->GetActualGridSize(Z_AXIS);
    }

    /// Access elements in the same way used in
    /// the computation kernel step.
    Timer *timer = Timer::GetInstance();
    timer->StartTimer("ComputationKernel::FirstTouch");

    Device::MemSet(ptr, 0.0f, nx * ny * nz * sizeof(float));

    timer->StopTimer("ComputationKernel::FirstTouch");
}
