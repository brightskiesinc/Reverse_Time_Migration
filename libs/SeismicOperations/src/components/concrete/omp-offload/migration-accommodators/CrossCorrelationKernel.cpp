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

#include <omp.h>
#include <vector>
#include <cmath>

#define EPSILON 1e-20

using namespace std;
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
    GridBox *receiver_gridbox = mpGridBox;


    int wnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    int wnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();


    float *source_base = source_gridbox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();
    float *receiver_base = receiver_gridbox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();
    float *correlation_output_base = mpShotCorrelation->GetNativePointer();
    float *source_i_base = mpSourceIllumination->GetNativePointer();
    float *receive_i_base = mpReceiverIllumination->GetNativePointer();

    uint offset = mpParameters->GetHalfLength();

    int nxEnd = this->mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize() - offset;
    int nyEnd;
    int nzEnd = this->mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize() - offset;

    int y_start = 0;

    if (!_IS_2D) {
        y_start = offset;
        nyEnd = this->mpGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize() - offset;
    } else {
        y_start = 0;
        nyEnd = 1;
    }

    float *src_ptr, *rec_ptr, *correlation_output, *source_i, *receive_i;
    uint b_offset = 0;
    float value = 0;

    const uint block_x = mpParameters->GetBlockX();
    const uint block_y = mpParameters->GetBlockY();
    const uint block_z = mpParameters->GetBlockZ();

    int device_num = omp_get_default_device();
#pragma omp target is_device_ptr(source_base, receiver_base, correlation_output_base, source_i_base, receive_i_base) device(device_num)
#pragma omp teams  distribute parallel for collapse(3)
    for (int by = y_start; by < nyEnd; by += block_y) {
        for (int bz = offset; bz < nzEnd; bz += block_z) {
            for (int bx = offset; bx < nxEnd; bx += block_x) {

                int izEnd = fmin(bz + block_z, nzEnd);
                int iyEnd = fmin(by + block_y, nyEnd);
                int ixEnd = fmin(block_x, nxEnd - bx);


                for (int iy = by; iy < iyEnd; ++iy) {
                    for (int iz = bz; iz < izEnd; ++iz) {
                        b_offset = iy * wnx * wnz + iz * wnx + bx;
                        src_ptr = source_base + b_offset;
                        rec_ptr = receiver_base + b_offset;
                        correlation_output = correlation_output_base + b_offset;
                        source_i = source_i_base + b_offset;
                        receive_i = receive_i_base + b_offset;

                        for (int ix = 0; ix < ixEnd; ++ix) {
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
    }
}


template<bool _IS_2D, COMPENSATION_TYPE _COMPENSATION_TYPE>
void CrossCorrelationKernel::Stack() {
    int wnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    int wnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

    int nx = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetActualAxisSize();
    int ny = this->mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetActualAxisSize();
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
    uint block_y = this->mpParameters->GetBlockY();
    uint offset = this->mpParameters->GetHalfLength() +
                  this->mpParameters->GetBoundaryLength();

    int x_end = this->mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize() - offset;
    int y_end = 1;
    int z_end = this->mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize() - offset;

    int y_start;
    if (ny > 1) {
        y_start = offset;
        y_end = this->mpGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize() - offset;
    } else {
        y_start = 0;
        y_end = 1;
    }


    int device_num = omp_get_default_device();

    float *input, *output, *input_src, *input_rcv;
    uint offset_full, offset_window;

#pragma omp target is_device_ptr(in, out, in_src, in_rcv) device(device_num)
#pragma omp teams distribute parallel for collapse(3)                    \
    num_teams((ny / block_y) * (nz / block_z) * (nx / block_x)) \
        thread_limit(block_y *block_z)
    for (int by = y_start; by < y_end; by += block_y) {
        for (int bz = offset; bz < z_end; bz += block_z) {
            for (int bx = offset; bx < x_end; bx += block_x) {

                int izEnd = fmin(bz + block_z, z_end);
                int iyEnd = fmin(by + block_y, y_end);
                int ixEnd = fmin(bx + block_x, x_end);

                for (int iy = by; iy < iyEnd; iy++) {
                    for (int iz = bz; iz < izEnd; iz++) {

                        offset_window = iy * wnx * wnz + iz * wnx;
                        offset_full = iy * nx * nz + iz * nx;

                        input = in + offset_window;
                        output = out + offset_full;

                        input_src = in_src + offset_window;

                        input_rcv = in_rcv + offset_window;

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
        }
    }
}
