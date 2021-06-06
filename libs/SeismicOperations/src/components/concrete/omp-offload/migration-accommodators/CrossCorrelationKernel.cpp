/*
 * CrossCorrelationKernel.cpp
 *
 *  Created on: Nov 29, 2020
 *      Author: mingy
 */

#include "operations/components/independents/concrete/migration-accommodators/CrossCorrelationKernel.hpp"
#include <memory-manager/MemoryManager.h>

#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include <omp.h>
#include <vector>


using namespace std;
using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;

#define EPSILON 1e-20

//optimized version

template void CrossCorrelationKernel::Correlation<true, NO_COMPENSATION>(GridBox *apGridBox);

template void CrossCorrelationKernel::Correlation<false, NO_COMPENSATION>(GridBox *apGridBox);

template void CrossCorrelationKernel::Correlation<true, COMBINED_COMPENSATION>(GridBox *apGridBox);

template void CrossCorrelationKernel::Correlation<false, COMBINED_COMPENSATION>(GridBox *apGridBox);

template<bool _IS_2D, COMPENSATION_TYPE _COMPENSATION_TYPE>
void CrossCorrelationKernel::Correlation(GridBox *apGridBox) {

    GridBox *source_gridbox = apGridBox;
    GridBox *receiver_gridbox = mpGridBox;

    int wnx = mpGridBox->GetActualWindowSize(X_AXIS);
    int wnz = mpGridBox->GetActualWindowSize(Z_AXIS);


    float *source_base = source_gridbox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();
    float *receiver_base = receiver_gridbox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();
    float *correlation_output_base = mpShotCorrelation->GetNativePointer();
    float *source_i_base = mpSourceIllumination->GetNativePointer();
    float *receive_i_base = mpReceiverIllumination->GetNativePointer();

    uint offset = mpParameters->GetHalfLength();
    int nxEnd = mpGridBox->GetLogicalWindowSize(X_AXIS) - offset;
    int nzEnd = mpGridBox->GetLogicalWindowSize(Z_AXIS) - offset;

    const uint block_x = mpParameters->GetBlockX();
    const uint block_z = mpParameters->GetBlockZ();

    int device_num = omp_get_default_device();

#pragma omp target data map(nzEnd, nxEnd, block_z, block_x)
#pragma omp target                                                                                      \
    is_device_ptr(source_base, receiver_base, correlation_output_base, source_i_base, receive_i_base)   \
    device(device_num)
#pragma omp teams distribute collapse(2)
    for (int bz = offset; bz < nzEnd; bz += block_z) {
        for (int bx = offset; bx < nxEnd; bx += block_x) {

            int izEnd = fmin(bz + block_z, nzEnd);
            int ixEnd = fmin(block_x, nxEnd - bx);

#pragma omp parallel for collapse(1)
            for (int iz = bz; iz < izEnd; ++iz) {
                uint b_offset = iz * wnx + bx;
                float *src_ptr = source_base + b_offset;
                float *rec_ptr = receiver_base + b_offset;
                float *correlation_output = correlation_output_base + b_offset;
                float *source_i = source_i_base + b_offset;
                float *receive_i = receive_i_base + b_offset;

#pragma vector aligned
#pragma ivdep
                for (int ix = 0; ix < ixEnd; ++ix) {
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

void CrossCorrelationKernel::Stack() {
    int nx = this->mpGridBox->GetActualGridSize(X_AXIS);
    int nz = this->mpGridBox->GetActualGridSize(Z_AXIS);

    int wnx = this->mpGridBox->GetActualWindowSize(X_AXIS);
    int wnz = this->mpGridBox->GetActualWindowSize(Z_AXIS);

    int constant = this->mpGridBox->GetWindowStart(X_AXIS) +
                   this->mpGridBox->GetWindowStart(Z_AXIS) * nx;

    float *in = this->mpShotCorrelation->GetNativePointer();
    float *out = this->mpTotalCorrelation->GetNativePointer() + constant;

    float *in_src = this->mpSourceIllumination->GetNativePointer();
    float *out_src = this->mpTotalSourceIllumination->GetNativePointer() + constant;

    float *in_rcv = this->mpReceiverIllumination->GetNativePointer();
    float *out_rcv = this->mpTotalReceiverIllumination->GetNativePointer() + constant;

    uint block_x = this->mpParameters->GetBlockX();
    uint block_z = this->mpParameters->GetBlockZ();
    uint offset = this->mpParameters->GetHalfLength() +
                  this->mpParameters->GetBoundaryLength();

    int x_end = this->mpGridBox->GetLogicalWindowSize(X_AXIS) - offset;
    int z_end = this->mpGridBox->GetLogicalWindowSize(Z_AXIS) - offset;

    int device_num = omp_get_default_device();

#pragma omp target is_device_ptr(in, out, in_src, out_src, in_rcv, out_rcv) device(device_num)
#pragma omp teams distribute collapse(2)                    \
    num_teams((nz / block_z) * (nx / block_x)) thread_limit(block_z)
    for (int bz = offset; bz < z_end; bz += block_z) {
        for (int bx = offset; bx < x_end; bx += block_x) {

            int izEnd = fmin(bz + block_z, z_end);
            int ixEnd = fmin(bx + block_x, x_end);

#pragma omp parallel for collapse(1) schedule(static, 1)
            for (int iz = bz; iz < izEnd; iz++) {
                uint offset_window = iz * wnx;
                uint offset = iz * nx;

                float *input = in + offset_window;
                float *output = out + offset;

                float *input_src = in_src + offset_window;
                float *output_src = out_src + offset;

                float *input_rcv = in_rcv + offset_window;
                float *output_rcv = out_rcv + offset;
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
