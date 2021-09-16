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

#include <operations/components/independents/concrete/migration-accommodators/CrossCorrelationKernel.hpp>

#include <bs/timer/api/cpp/BSTimer.hpp>

#include <cmath>
#include <vector>

#define EPSILON 1e-20f

using namespace std;
using namespace bs::timer;
using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;


template void CrossCorrelationKernel::Correlation<true, NO_COMPENSATION>(GridBox *apGridBox);

template void CrossCorrelationKernel::Correlation<false, NO_COMPENSATION>(GridBox *apGridBox);

template void CrossCorrelationKernel::Correlation<true, COMBINED_COMPENSATION>(GridBox *apGridBox);

template void CrossCorrelationKernel::Correlation<false, COMBINED_COMPENSATION>(GridBox *apGridBox);

template void CrossCorrelationKernel::Stack<true, NO_COMPENSATION>();

template void CrossCorrelationKernel::Stack<false, NO_COMPENSATION>();

template void CrossCorrelationKernel::Stack<true, COMBINED_COMPENSATION>();

template void CrossCorrelationKernel::Stack<false, COMBINED_COMPENSATION>();

template<bool _IS_2D, COMPENSATION_TYPE _COMPENSATION_TYPE>
void CrossCorrelationKernel::Correlation(GridBox *apGridBox) {

    GridBox *source_gridbox = apGridBox;
    GridBox *receiver_gridbox = this->mpGridBox;

    int wnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    int wnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

    int nx = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetActualAxisSize();
    int nz = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetActualAxisSize();

    float *source_base = source_gridbox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();
    float *receiver_base = receiver_gridbox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();
    float *corr_base = mpShotCorrelation->GetNativePointer();
    float *source_illumination_base = mpSourceIllumination->GetNativePointer();
    float *receiver_illumination_base = mpReceiverIllumination->GetNativePointer();

    float *src_ptr;
    float *rec_ptr;
    float *source_i;
    float *receive_i;
    float *correlation_output;
    uint offset = mpParameters->GetHalfLength();
    int nxEnd = this->mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize() - offset;
    int nzEnd = this->mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize() - offset;

    int size = (wnx - 2 * offset) * (wnz - 2 * offset);
    int flops_per_second = 3 * offset;

    if (_COMPENSATION_TYPE == COMBINED_COMPENSATION) {
        flops_per_second = 9 * offset;
    }

    ElasticTimer timer("Correlation::Correlate::Kernel",
                       size, 5, true,
                       flops_per_second);
    timer.Start();
#pragma omp parallel default(shared)
    {
        const uint block_x = mpParameters->GetBlockX();
        const uint block_y = mpParameters->GetBlockY();
        const uint block_z = mpParameters->GetBlockZ();

#pragma omp for schedule(static, 1) collapse(2)
        for (int bz = offset; bz < nzEnd; bz += block_z) {
            for (int bx = offset; bx < nxEnd; bx += block_x) {

                int izEnd = fmin(bz + block_z, nzEnd);
                int ixEnd = fmin(block_x, nxEnd - bx);

                for (int iz = bz; iz < izEnd; ++iz) {
                    uint b_offset = iz * wnx + bx;
                    src_ptr = source_base + b_offset;
                    rec_ptr = receiver_base + b_offset;
                    correlation_output = corr_base + b_offset;
                    source_i = source_illumination_base + b_offset;
                    receive_i = receiver_illumination_base + b_offset;

#pragma vector aligned
#pragma ivdep
                    for (int ix = 0; ix < ixEnd; ix++) {
                        float value;

                        value = src_ptr[ix] * rec_ptr[ix];
                        correlation_output[ix] += value;
                        if (_COMPENSATION_TYPE == COMBINED_COMPENSATION) {
                            source_i[ix] += src_ptr[ix] * src_ptr[ix];
                            receive_i[ix] += rec_ptr[ix] * rec_ptr[ix];
                        }
                    }
                }
            }
        }
    }
    timer.Stop();
}

template<bool _IS_2D, COMPENSATION_TYPE _COMPENSATION_TYPE>
void CrossCorrelationKernel::Stack() {

    int wnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    int wnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

    int nx = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetActualAxisSize();
    int nz = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetActualAxisSize();


    int constant = this->mpGridBox->GetWindowStart(X_AXIS) +
                   this->mpGridBox->GetWindowStart(Z_AXIS) * nx +
                   this->mpGridBox->GetWindowStart(Y_AXIS) * nx * nz;

    float *in = this->mpShotCorrelation->GetNativePointer();
    float *out = this->mpTotalCorrelation->GetNativePointer() + constant;

    float *in_src = this->mpSourceIllumination->GetNativePointer();
    float *in_rcv = this->mpReceiverIllumination->GetNativePointer();

    uint block_x = this->mpParameters->GetBlockX();
    uint block_z = this->mpParameters->GetBlockZ();

    uint offset = this->mpParameters->GetHalfLength() +
                  this->mpParameters->GetBoundaryLength();

    int x_end = this->mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize() - offset;
    int z_end = this->mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize() - offset;

    int size = (wnx - 2 * offset) * (wnz - 2 * offset);
    int flops_per_second = 2 * offset;
    if (_COMPENSATION_TYPE == COMBINED_COMPENSATION) {
        flops_per_second = 6 * offset;
    }

    float *input, *output, *input_src, *input_rcv;

    ElasticTimer timer("Correlation::Stack::Kernel",
                       size, 4, true,
                       flops_per_second);
    timer.Start();
#pragma omp parallel for schedule(static, 1) collapse(2)
    for (int bz = offset; bz < z_end; bz += block_z) {
        for (int bx = offset; bx < x_end; bx += block_x) {

            int izEnd = fmin(bz + block_z, z_end);
            int ixEnd = fmin(bx + block_x, x_end);

            for (int iz = bz; iz < izEnd; iz++) {
                uint offset_window = iz * wnx;
                uint offset = iz * nx;

                input = in + offset_window;
                output = out + offset;

                input_src = in_src + offset_window;

                input_rcv = in_rcv + offset_window;
#pragma ivdep
#pragma vector aligned
                for (int ix = bx; ix < ixEnd; ix++) {
                    if constexpr (_COMPENSATION_TYPE == COMBINED_COMPENSATION) {
                        output[ix] += (input[ix] / (sqrtf(input_src[ix] * input_rcv[ix]) + EPSILON));
                    } else {
                        output[ix] += input[ix];
                    }
                }
            }
        }
    }
    timer.Stop();
}
