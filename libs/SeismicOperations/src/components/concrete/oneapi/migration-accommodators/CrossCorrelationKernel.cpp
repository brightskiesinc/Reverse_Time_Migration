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

#include <operations/backend/OneAPIBackend.hpp>

#include <bs/timer/api/cpp/BSTimer.hpp>

#include <cstdlib>
#include <cstring>
#include <cmath>
#include <vector>

#define EPSILON 1e-20f

using namespace cl::sycl;
using namespace std;
using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;
using namespace operations::backend;
using namespace bs::timer;


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


    int wnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    int wny = this->mpGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
    int wnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

    int compute_nx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetComputationAxisSize();
    int compute_ny = this->mpGridBox->GetWindowAxis()->GetYAxis().GetComputationAxisSize();
    int compute_nz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetComputationAxisSize();

    int block_x = mpParameters->GetBlockX();
    int block_y = mpParameters->GetBlockY();
    int block_z = mpParameters->GetBlockZ();
    int half_length = mpParameters->GetHalfLength();

    int y_offset = half_length;
    if (_IS_2D) {
        y_offset = 0;
    }
    int size = (wnx - 2 * half_length) * (wnz - 2 * half_length);
    int flops_per_second = 3 * half_length;
    if (_COMPENSATION_TYPE == COMPENSATION_TYPE::COMBINED_COMPENSATION) {
        flops_per_second = 9 * half_length;
    }

    ElasticTimer timer("Correlation::Correlate::Kernel",
                       size, 5, true,
                       flops_per_second);

    float *source = apGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();
    float *receiver = mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();
    timer.Start();
    if (OneAPIBackend::GetInstance()->GetAlgorithm() == SYCL_ALGORITHM::CPU) {
        float *output_buffer = mpShotCorrelation->GetNativePointer();
        float *src_buffer = mpSourceIllumination->GetNativePointer();
        float *dest_buffer = mpReceiverIllumination->GetNativePointer();

        OneAPIBackend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
            size_t num_groups = OneAPIBackend::GetInstance()->GetWorkgroupNumber();
            size_t wgsize = OneAPIBackend::GetInstance()->GetWorkgroupSize();
            size_t z_stride = compute_nz / num_groups;
            auto global_range = range<1>(num_groups);
            auto local_range = range<1>(wgsize);

            const int hl = mpParameters->GetHalfLength();

            cgh.parallel_for_work_group(
                    global_range, local_range, [=](group<1> grp) {
                        size_t z_id = grp.get_id(0) * z_stride + hl;
                        size_t end_z = (z_id + z_stride) < (compute_nz + hl) ? (z_id + z_stride) : (compute_nz + hl);
                        grp.parallel_for_work_item([&](h_item<1> it) {
                            for (size_t iz = z_id; iz < end_z; iz++) {
                                size_t offset = iz * wnx + it.get_local_id(0);
                                for (size_t ix = hl; ix < hl + compute_nx; ix += wgsize) {
                                    size_t idx = offset + ix;
                                    output_buffer[idx] += source[idx] * receiver[idx];
                                    if (_COMPENSATION_TYPE == COMPENSATION_TYPE::COMBINED_COMPENSATION) {
                                        src_buffer[idx] += source[idx] * source[idx];
                                        dest_buffer[idx] += receiver[idx] * receiver[idx];
                                    }
                                }
                            }
                        });
                    });
        });
    } else {
        OneAPIBackend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {

            auto global_range = range<3>(compute_ny, compute_nz, compute_nx);
            auto local_range = range<3>(1, 1, block_x);
            auto starting_offset = id<3>(y_offset, half_length, half_length);
            auto global_nd_range = nd_range<3>(global_range,
                                               local_range,
                                               starting_offset);

            float *output_buffer = mpShotCorrelation->GetNativePointer();
            float *src_buffer = mpSourceIllumination->GetNativePointer();
            float *dest_buffer = mpReceiverIllumination->GetNativePointer();
            cgh.parallel_for(global_nd_range, [=](nd_item<3> it) {

                int idx = it.get_global_id(0) * wnz * wnx + it.get_global_id(1) * wnx + it.get_global_id(2);
                output_buffer[idx] += source[idx] * receiver[idx];

                if (_COMPENSATION_TYPE == COMPENSATION_TYPE::COMBINED_COMPENSATION) {
                    src_buffer[idx] += source[idx] * source[idx];
                    dest_buffer[idx] += receiver[idx] * receiver[idx];
                }

            });
        });
    }
    OneAPIBackend::GetInstance()->GetDeviceQueue()->wait();
    timer.Stop();
}

template<bool _IS_2D, COMPENSATION_TYPE _COMPENSATION_TYPE>
void CrossCorrelationKernel::Stack() {

    int nx = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetActualAxisSize();
    int ny = this->mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetActualAxisSize();
    int nz = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetActualAxisSize();
    int wnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    int wny = this->mpGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
    int wnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();
    int orig_x = this->mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize();
    int orig_y = this->mpGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize();
    int orig_z = this->mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize();

    int offset = this->mpParameters->GetHalfLength() + this->mpParameters->GetBoundaryLength();
    int constant_offset = offset + offset * nx;
    int constant_offset_win = offset + offset * wnx;
    if (!_IS_2D) {
        constant_offset += offset * nx * nz;
        constant_offset_win += offset * wnx * wnz;
    }
    int size = (wnx - 2 * offset) * (wnz - 2 * offset);
    int flops_per_second = 6 * offset;
    if (_COMPENSATION_TYPE == NO_COMPENSATION) {
        flops_per_second = 2 * offset;
    }
    ElasticTimer timer("Correlation::Stack::Kernel",
                       size, 4, true,
                       flops_per_second);
    size_t sizeTotal = nx * nz * ny;
    timer.Start();
    OneAPIBackend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
        auto global_range = range<3>(orig_x - 2 * offset, orig_z - 2 * offset,
                                     orig_y);
        int wsx = mpGridBox->GetWindowStart(X_AXIS);
        int wsz = mpGridBox->GetWindowStart(Z_AXIS);
        int wsy = mpGridBox->GetWindowStart(Y_AXIS);
        float *stack_buf = mpTotalCorrelation->GetNativePointer() + wsx + wsz * nx + wsy * nx * nz;
        float *cor_buf = mpShotCorrelation->GetNativePointer();
        float *cor_src = mpSourceIllumination->GetNativePointer();
        float *cor_rcv = mpReceiverIllumination->GetNativePointer();
        if (_COMPENSATION_TYPE == NO_COMPENSATION) {
            cgh.parallel_for(
                    global_range, [=](id<3> idx) {
                        uint offset_window = idx[0] + idx[1] * wnx + idx[2] * wnx * wnz
                                             + constant_offset_win;
                        uint offset = idx[0] + idx[1] * nx + idx[2] * nx * nz
                                      + constant_offset;
                        stack_buf[offset] += cor_buf[offset_window];
                    });
        } else {
            cgh.parallel_for(
                    global_range, [=](id<3> idx) {
                        uint offset_window = idx[0] + idx[1] * wnx + idx[2] * wnx * wnz
                                             + constant_offset_win;
                        uint offset = idx[0] + idx[1] * nx + idx[2] * nx * nz
                                      + constant_offset;
                        stack_buf[offset] += (cor_buf[offset_window] /
                                              (sqrtf(cor_src[offset_window] * cor_rcv[offset_window]) + EPSILON));
                    });
        }
    });
    OneAPIBackend::GetInstance()->GetDeviceQueue()->wait();
    timer.Stop();
}
