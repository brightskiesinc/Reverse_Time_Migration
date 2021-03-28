/*
 * CrossCorrelationKernel.cpp
 *
 *  Created on: Nov 29, 2020
 *      Author: aayyad
 */

#include "operations/components/independents/concrete/migration-accommodators/CrossCorrelationKernel.hpp"

#include <memory-manager/MemoryManager.h>

#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include <omp.h>
#include <vector>

#define EPSILON 1e-20

using namespace std;
using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;

template void CrossCorrelationKernel::Correlation<true, NO_COMPENSATION>(GridBox *apGridBox);

template void CrossCorrelationKernel::Correlation<false, NO_COMPENSATION>(GridBox *apGridBox);

template void CrossCorrelationKernel::Correlation<true, COMBINED_COMPENSATION>(GridBox *apGridBox);

template void CrossCorrelationKernel::Correlation<false, COMBINED_COMPENSATION>(GridBox *apGridBox);

template<bool _IS_2D, COMPENSATION_TYPE _COMPENSATION_TYPE>
void CrossCorrelationKernel::Correlation(GridBox *apGridBox) {

    GridBox *source_gridbox = apGridBox;
    GridBox *receiver_gridbox = mpGridBox;

    int nx = mpGridBox->GetActualGridSize(X_AXIS);
    int ny = mpGridBox->GetActualGridSize(Y_AXIS);
    int nz = mpGridBox->GetActualGridSize(Z_AXIS);

    int wnx = mpGridBox->GetActualWindowSize(X_AXIS);
    int wny = mpGridBox->GetActualWindowSize(Y_AXIS);
    int wnz = mpGridBox->GetActualWindowSize(Z_AXIS);

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
    int nxEnd = mpGridBox->GetLogicalWindowSize(X_AXIS) - offset;
    int nyEnd;
    int nzEnd = mpGridBox->GetLogicalWindowSize(Z_AXIS) - offset;
    int y_start;
    if (!_IS_2D) {
        y_start = offset;
        nyEnd = mpGridBox->GetLogicalWindowSize(Y_AXIS) - offset;
    } else {
        y_start = 0;
        nyEnd = 1;
    }
#pragma omp parallel default(shared)
    {
        const uint block_x = mpParameters->GetBlockX();
        const uint block_y = mpParameters->GetBlockY();
        const uint block_z = mpParameters->GetBlockZ();

#pragma omp for schedule(static, 1) collapse(2)
        for (int by = y_start; by < nyEnd; by += block_y) {
            for (int bz = offset; bz < nzEnd; bz += block_z) {
                for (int bx = offset; bx < nxEnd; bx += block_x) {

                    int izEnd = fmin(bz + block_z, nzEnd);
                    int iyEnd = fmin(by + block_y, nyEnd);
                    int ixEnd = fmin(block_x, nxEnd - bx);

                    for (int iy = by; iy < iyEnd; ++iy) {
                        for (int iz = bz; iz < izEnd; ++iz) {
                            uint b_offset = iy * wnx * wnz + iz * wnx + bx;
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
        }
    }
}

void CrossCorrelationKernel::Stack() {
    int nx = this->mpGridBox->GetActualGridSize(X_AXIS);
    int ny = this->mpGridBox->GetActualGridSize(Y_AXIS);
    int nz = this->mpGridBox->GetActualGridSize(Z_AXIS);

    int wnx = this->mpGridBox->GetActualWindowSize(X_AXIS);
    int wny = this->mpGridBox->GetActualWindowSize(Y_AXIS);
    int wnz = this->mpGridBox->GetActualWindowSize(Z_AXIS);

    int constant = this->mpGridBox->GetWindowStart(X_AXIS) +
                   this->mpGridBox->GetWindowStart(Z_AXIS) * nx +
                   this->mpGridBox->GetWindowStart(Y_AXIS) * nx * nz;

    float *in = this->mpShotCorrelation->GetNativePointer();
    float *out = this->mpTotalCorrelation->GetNativePointer() + constant;

    float *in_src = this->mpSourceIllumination->GetNativePointer();
    float *out_src = this->mpTotalSourceIllumination->GetNativePointer() + constant;

    float *in_rcv = this->mpReceiverIllumination->GetNativePointer();
    float *out_rcv = this->mpTotalReceiverIllumination->GetNativePointer() + constant;

    uint block_x = this->mpParameters->GetBlockX();
    uint block_z = this->mpParameters->GetBlockZ();
    uint block_y = this->mpParameters->GetBlockY();
    uint offset = this->mpParameters->GetHalfLength() +
                  this->mpParameters->GetBoundaryLength();

    int x_end = this->mpGridBox->GetLogicalWindowSize(X_AXIS) - offset;
    int y_end;
    int z_end = this->mpGridBox->GetLogicalWindowSize(Z_AXIS) - offset;

    int y_start;
    if (ny > 1) {
        y_start = offset;
        y_end = this->mpGridBox->GetLogicalWindowSize(Y_AXIS) - offset;
    } else {
        y_start = 0;
        y_end = 1;
    }

    float *input, *output, *input_src, *output_src, *input_rcv, *output_rcv;

#pragma omp parallel for schedule(static, 1) collapse(2)
    for (int by = y_start; by < y_end; by += block_y) {
        for (int bz = offset; bz < z_end; bz += block_z) {
            for (int bx = offset; bx < x_end; bx += block_x) {

                int izEnd = fmin(bz + block_z, z_end);
                int iyEnd = fmin(by + block_y, y_end);
                int ixEnd = fmin(bx + block_x, x_end);

                for (int iy = by; iy < iyEnd; iy++) {
                    for (int iz = bz; iz < izEnd; iz++) {
                        uint offset_window = iy * wnx * wnz + iz * wnx;
                        uint offset = iy * nx * nz + iz * nx;

                        input = in + offset_window;
                        output = out + offset;

                        input_src = in_src + offset_window;
                        output_src = out_src + offset;

                        input_rcv = in_rcv + offset_window;
                        output_rcv = out_rcv + offset;
#pragma ivdep
#pragma vector aligned
                        for (int ix = bx; ix < ixEnd; ix++) {
                            output[ix] += input[ix];
                            output_rcv[ix] += input_rcv[ix];
                            output_src[ix] += input_src[ix];
                        }
                    }
                }
            }
        }
    }
}

