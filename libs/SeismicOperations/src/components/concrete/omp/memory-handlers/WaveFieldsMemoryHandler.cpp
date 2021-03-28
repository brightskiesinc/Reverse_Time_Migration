//
// Created by zeyad-osama on 26/09/2020.
//

#include <operations/components/dependents/concrete/memory-handlers/WaveFieldsMemoryHandler.hpp>

#include <cmath>

using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;


void WaveFieldsMemoryHandler::FirstTouch(
        float *ptr, GridBox *apGridBox, bool enable_window) {
    float *curr_base = ptr;
    float *curr;
    int nx, ny, nz;

    if (enable_window) {
        nx = apGridBox->GetLogicalWindowSize(X_AXIS);
        ny = apGridBox->GetLogicalWindowSize(Y_AXIS);
        nz = apGridBox->GetLogicalWindowSize(Z_AXIS);
    } else {
        nx = apGridBox->GetLogicalGridSize(X_AXIS);
        ny = apGridBox->GetLogicalGridSize(Y_AXIS);
        nz = apGridBox->GetLogicalGridSize(Z_AXIS);
    }

    int nxnz = nx * nz;
    int nxEnd = nx - this->mpParameters->GetHalfLength();
    int nyEnd = 1;
    int nzEnd = nz - this->mpParameters->GetHalfLength();
    int y_start = 0;
    if (ny > 1) {
        y_start = this->mpParameters->GetHalfLength();
        nyEnd = ny - this->mpParameters->GetHalfLength();
    }
    uint half_length = this->mpParameters->GetHalfLength();
    int block_x = this->mpParameters->GetBlockX();
    int block_y = this->mpParameters->GetBlockY();
    int block_z = this->mpParameters->GetBlockZ();

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

#pragma omp parallel for schedule(static, 1) collapse(2)
    for (int by = y_start; by < nyEnd; by += block_y) {
        for (int bz = half_length; bz < nzEnd; bz += block_z) {
            for (int bx = half_length; bx < nxEnd; bx += block_x) {
                int izEnd = fmin(bz + block_z, nzEnd);
                int iyEnd = fmin(by + block_y, nyEnd);
                int ixEnd = fmin(block_x, nxEnd - bx);
                for (int iy = by; iy < iyEnd; ++iy) {
                    for (int iz = bz; iz < izEnd; ++iz) {
                        curr = curr_base + iy * nxnz + iz * nx + bx;
#pragma ivdep
                        for (int ix = 0; ix < ixEnd; ++ix) {
                            curr[ix] = 0;
                        }
                    }
                }
            }
        }
    }
    memset(ptr, 0, sizeof(float) * nx * nz * ny);
    timer->StopTimer("ComputationKernel::FirstTouch");
}
