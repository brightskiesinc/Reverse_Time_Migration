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

#include <cmath>

#include <bs/timer/api/cpp/BSTimer.hpp>
#include <operations/components/dependents/concrete/memory-handlers/WaveFieldsMemoryHandler.hpp>

using namespace std;
using namespace bs::timer;
using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;


void
WaveFieldsMemoryHandler::FirstTouch(
        float *ptr, GridBox *apGridBox, bool enable_window) {

    float *curr_base = ptr;
    float *curr;
    int nx, ny, nz;

    if (enable_window) {

        nx = apGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize();
        ny = apGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize();
        nz = apGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize();

    } else {

        nx = apGridBox->GetAfterSamplingAxis()->GetXAxis().GetLogicalAxisSize();
        ny = apGridBox->GetAfterSamplingAxis()->GetYAxis().GetLogicalAxisSize();
        nz = apGridBox->GetAfterSamplingAxis()->GetZAxis().GetLogicalAxisSize();
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

        nx = apGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
        ny = apGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
        nz = apGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

    } else {

        nx = apGridBox->GetAfterSamplingAxis()->GetXAxis().GetActualAxisSize();
        ny = apGridBox->GetAfterSamplingAxis()->GetYAxis().GetActualAxisSize();
        nz = apGridBox->GetAfterSamplingAxis()->GetZAxis().GetActualAxisSize();
    }

    /// Access elements in the same way used in
    /// the computation kernel step.
    ElasticTimer timer("ComputationKernel::FirstTouch");
    timer.Start();
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
    timer.Stop();
}
