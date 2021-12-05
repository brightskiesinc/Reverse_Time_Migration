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

#include <bs/base/api/cpp/BSBase.hpp>
#include "operations/components/independents/concrete/computation-kernels/isotropic/StaggeredComputationKernel.hpp"
#include "operations/components/dependents/concrete/memory-handlers/WaveFieldsMemoryHandler.hpp"
#include "operations/components/independents/concrete/computation-kernels/BaseComputationHelpers.hpp"
#include <bs/timer/api/cpp/BSTimer.hpp>


#include "fstream"

using namespace cl::sycl;
using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;
using namespace bs::base::backend;
using namespace std;
using namespace bs::timer;


FORWARD_DECLARE_COMPUTE_TEMPLATE(StaggeredComputationKernel, ComputePressure)

FORWARD_DECLARE_COMPUTE_TEMPLATE(StaggeredComputationKernel, ComputeVelocity)


template<KERNEL_MODE KERNEL_MODE_, bool IS_2D_, HALF_LENGTH HALF_LENGTH_>
void StaggeredComputationKernel::ComputePressure() {

    float *curr_base = this->mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();
    float *next_base = this->mpGridBox->Get(WAVE | GB_PRSS | NEXT | DIR_Z)->GetNativePointer();

    float *particle_vel_x = this->mpGridBox->Get(WAVE | GB_PRTC | CURR | DIR_X)->GetNativePointer();
    float *particle_vel_y;
    float *particle_vel_z = this->mpGridBox->Get(WAVE | GB_PRTC | CURR | DIR_Z)->GetNativePointer();

    float *vel_base = this->mpGridBox->Get(PARM | WIND | GB_VEL)->GetNativePointer();
    float *den_base = this->mpGridBox->Get(PARM | WIND | GB_DEN)->GetNativePointer();

    int wnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    int wny = this->mpGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
    int wnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

    float dx = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetCellDimension();
    float dy = this->mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetCellDimension();
    float dz = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetCellDimension();

    int block_x = this->mpParameters->GetBlockX();
    int block_y = this->mpParameters->GetBlockY();
    int block_z = this->mpParameters->GetBlockZ();

    int wnxnz = wnx * wnz;
    int nx = wnx;
    int nz = wnz;
    int nxnz = nx * nz;
    int size = (wnx - 2 * HALF_LENGTH_) * (wnz - 2 * HALF_LENGTH_);

    int flops_per_pressure = 6 * HALF_LENGTH_ + 3;

    int num_of_arrays_pressure = 5;

    ElasticTimer timer("ComputationKernel::ComputePressure",
                       size,
                       num_of_arrays_pressure,
                       true,
                       flops_per_pressure);
    timer.Start();

    if (Backend::GetInstance()->GetAlgorithm() == SYCL_ALGORITHM::CPU) {

        Backend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
            size_t num_groups = Backend::GetInstance()->GetWorkgroupNumber();
            size_t wgsize = Backend::GetInstance()->GetWorkgroupSize();

            int compute_nz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetComputationAxisSize();
            int compute_nx = mpParameters->GetHalfLength() +
                             this->mpGridBox->GetWindowAxis()->GetXAxis().GetComputationAxisSize();
            size_t z_stride = (compute_nz + num_groups - 1) / num_groups;
            auto global_range = range<1>(num_groups);
            auto local_range = range<1>(wgsize);
            const size_t wnx = nx;
            const size_t wnz = nz;
            float *coeff_x = mpCoeff->GetHostPointer();
            float *coeff_z = mpCoeff->GetHostPointer();
            const float *current = curr_base;
            const float *vel = vel_base;
            float *next = next_base;
            float *vel_x = particle_vel_x;
            float *vel_z = particle_vel_z;
            float *den = den_base;

            cgh.parallel_for_work_group(global_range, local_range, [=](group<1> grp) {
                size_t z_id = grp.get_id(0) * z_stride + HALF_LENGTH_;
                size_t end_z =
                        (z_id + z_stride) < (compute_nz + HALF_LENGTH_) ?
                        (z_id + z_stride) : (compute_nz + HALF_LENGTH_);
                grp.parallel_for_work_item([&](h_item<1> it) {
                    for (size_t iz = z_id; iz < end_z; iz++) {
                        size_t offset = iz * wnx + it.get_local_id(0);
                        for (size_t ix = HALF_LENGTH_; ix < compute_nx; ix += wgsize) {
                            float value_x = 0;
                            float value_z = 0;
                            size_t idx = offset + ix;

                            DERIVE_SEQ_AXIS(idx, 0, 1, -, vel_x, coeff_x, value_x)
                            DERIVE_JUMP_AXIS(idx, wnx, 0, 1, -, vel_z, coeff_z, value_z)

                            if constexpr (KERNEL_MODE_ != KERNEL_MODE::INVERSE) {

                                next[idx] = current[idx] - vel[idx] * ((value_x / dx) + (value_z / dz));
                            } else {
                                next[idx] = current[idx] + vel[idx] * ((value_x / dx) + (value_z / dz));
                            }
                        }
                    }
                });
            });
        });
    } else if (Backend::GetInstance()->GetAlgorithm() == SYCL_ALGORITHM::GPU_SHARED) {
        int compute_nz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetComputationAxisSize();
        int compute_nx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetComputationAxisSize();

        Backend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
            auto global_range = range<2>(compute_nz, compute_nx);
            auto local_range = range<2>(block_z, block_x);
            auto workgroup_range = nd_range<2>(global_range, local_range);

            const float *current = curr_base;
            float *next = next_base;
            const float *vel = vel_base;
            float *vel_x = particle_vel_x;
            float *vel_z = particle_vel_z;
            const float *coeff = mpCoeff->GetNativePointer();
            float *den = den_base;

            const int idx_range = block_z;
            const int local_nx = block_x + 2 * HALF_LENGTH_;
            const int local_nz = block_z + 2 * HALF_LENGTH_;

            auto localRange_ptr_cur = range<1>(((block_x + (2 * HALF_LENGTH_)) *
                                                (block_z + (2 * HALF_LENGTH_))));

            accessor<float, 1, access::mode::read_write, access::target::local> tab(localRange_ptr_cur, cgh);
            cgh.parallel_for(workgroup_range, [=](nd_item<2> it) {

                int idx = it.get_global_id(1) + HALF_LENGTH_ + (it.get_global_id(0) + HALF_LENGTH_) * nx;
                int id0 = it.get_local_id(1);
                int id1 = it.get_local_id(0);

                float coeff_x_device[HALF_LENGTH_];
                float coeff_z_device[HALF_LENGTH_];
                for (int iter = 0; iter < HALF_LENGTH_; iter++) {
                    coeff_x_device[iter] = coeff[iter];
                    coeff_z_device[iter] = coeff[iter];
                }

                float value_x = 0;
                float value_z = 0;

                DERIVE_SEQ_AXIS(idx, 0, 1, -, vel_x, coeff_x_device, value_x)
                DERIVE_JUMP_AXIS(idx, wnx, 0, 1, -, vel_z, coeff_z_device, value_z)

                if constexpr (KERNEL_MODE_ != KERNEL_MODE::INVERSE) {
                    next[idx] = current[idx] - vel[idx] * ((value_x / dx) + (value_z / dz));
                } else {
                    next[idx] = current[idx] + vel[idx] * ((value_x / dx) + (value_z / dz));
                }
            });
        });
    } else if (Backend::GetInstance()->GetAlgorithm() == SYCL_ALGORITHM::GPU_SEMI_SHARED) {
        int compute_nz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetComputationAxisSize() / block_z;
        int compute_nx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetComputationAxisSize();

        Backend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
            auto global_range = range<2>(compute_nz, compute_nx);
            auto local_range = range<2>(1, block_x);
            auto workgroup_range = nd_range<2>(global_range, local_range);
            const float *current = curr_base;
            float *next = next_base;
            const float *vel = vel_base;
            float *vel_x = particle_vel_x;
            float *vel_z = particle_vel_z;
            float *den = den_base;
            const int idx_range = block_z;
            const int local_nx = block_x + 2 * HALF_LENGTH_;
            const float *coeff = mpCoeff->GetNativePointer();

            auto localRange_ptr_cur = range<1>(((block_x + (2 * HALF_LENGTH_)) *
                                                (block_z + (2 * HALF_LENGTH_))));

            accessor<float, 1, access::mode::read_write, access::target::local> tab(
                    localRange_ptr_cur, cgh);
            cgh.parallel_for(workgroup_range,
                             [=](nd_item<2> it) {

                                 int idx = it.get_global_id(1) + HALF_LENGTH_ +
                                           (it.get_global_id(0) * idx_range +
                                            HALF_LENGTH_) * nx;
                                 size_t id0 = it.get_local_id(1);
                                 size_t offset = (id0 + HALF_LENGTH_) +
                                                 HALF_LENGTH_ * local_nx;
                                 float coeff_x_device[HALF_LENGTH_];
                                 float coeff_z_device[HALF_LENGTH_];

                                 for (unsigned int iter = 0;
                                      iter < HALF_LENGTH_; iter++) {
                                     coeff_x_device[iter] = coeff[iter];
                                     coeff_z_device[iter] = coeff[iter];
                                 }

                                 for (int i = 0; i < idx_range; i++) {
                                     float value_x = 0, value_z = 0;

                                     DERIVE_SEQ_AXIS(idx, 0, 1, -, vel_x, coeff_x_device, value_x)
                                     DERIVE_JUMP_AXIS(idx, wnx, 0, 1, -, vel_z, coeff_z_device, value_z)

                                     if constexpr (KERNEL_MODE_ != KERNEL_MODE::INVERSE) {
                                         next[idx] = current[idx] - vel[idx] * ((value_x / dx) + (value_z / dz));
                                     } else {
                                         next[idx] = current[idx] + vel[idx] * ((value_x / dx) + (value_z / dz));
                                     }

                                     idx += nx;
                                 }
                             });
        });
    } else if (Backend::GetInstance()->GetAlgorithm() == SYCL_ALGORITHM::GPU) {
        int compute_nz = mpGridBox->GetWindowAxis()->GetZAxis().GetComputationAxisSize() / block_z;
        int compute_nx = mpGridBox->GetWindowAxis()->GetXAxis().GetComputationAxisSize();

        Backend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
            auto global_range = range<2>(compute_nz, compute_nx);
            auto local_range = range<2>(1, block_x);
            auto workgroup_range = nd_range<2>(global_range, local_range);
            const float *current = curr_base;
            float *next = next_base;
            float *vel_x = particle_vel_x;
            float *vel_z = particle_vel_z;
            float *vel = vel_base;
            float *den = den_base;
            const int idx_range = block_z;
            const int local_nx = block_x + 2 * HALF_LENGTH_;
            const int local_nz = block_x + 2 * HALF_LENGTH_;
            const int pad = 0;
            float *coeff = mpCoeff->GetNativePointer();

            auto localRange_ptr_cur =
                    range<1>((block_x + (2 * HALF_LENGTH_) + pad));

            accessor<float, 1, access::mode::read_write, access::target::local> tab(
                    localRange_ptr_cur, cgh);
            cgh.parallel_for(workgroup_range,
                             [=](nd_item<2> it) {

                                 int idx = it.get_global_id(1) + HALF_LENGTH_ +
                                           (it.get_global_id(0) * idx_range +
                                            HALF_LENGTH_) * nx;
                                 int id0 = it.get_local_id(1);
                                 float coeff_x_device[HALF_LENGTH_];
                                 float coeff_z_device[HALF_LENGTH_];

                                 for (int iter = 0; iter < HALF_LENGTH_; iter++) {
                                     coeff_x_device[iter] = coeff[iter];
                                     coeff_z_device[iter] = coeff[iter];
                                 }

                                 for (int i = 0; i < idx_range; i++) {

                                     float value_x = 0, value_z = 0;

                                     DERIVE_SEQ_AXIS(idx, 0, 1, -, vel_x, coeff_x_device, value_x)
                                     DERIVE_JUMP_AXIS(idx, wnx, 0, 1, -, vel_z, coeff_z_device, value_z)

                                     if constexpr (KERNEL_MODE_ != KERNEL_MODE::INVERSE) {
                                         next[idx] = current[idx] - vel[idx] * ((value_x / dx) + (value_z / dz));
                                     } else {
                                         next[idx] = current[idx] + vel[idx] * ((value_x / dx) + (value_z / dz));
                                     }

                                     idx += nx;
                                 }

                             });
        });
    }

    Backend::GetInstance()->GetDeviceQueue()->wait();
    timer.Stop();

}

template<KERNEL_MODE KERNEL_MODE_, bool IS_2D_, HALF_LENGTH HALF_LENGTH_>
void StaggeredComputationKernel::ComputeVelocity() {

    float *curr_base = mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();

    float *particle_vel_x = this->mpGridBox->Get(WAVE | GB_PRTC | CURR | DIR_X)->GetNativePointer();
    float *particle_vel_y;
    float *particle_vel_z = this->mpGridBox->Get(WAVE | GB_PRTC | CURR | DIR_Z)->GetNativePointer();

    float *den_base = mpGridBox->Get(PARM | WIND | GB_DEN)->GetNativePointer();
    float *vel_base = mpGridBox->Get(PARM | WIND | GB_VEL)->GetNativePointer();

    int wnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    int wny = this->mpGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
    int wnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

    float dx = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetCellDimension();
    float dy = this->mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetCellDimension();
    float dz = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetCellDimension();

    int block_x = this->mpParameters->GetBlockX();
    int block_y = this->mpParameters->GetBlockY();
    int block_z = this->mpParameters->GetBlockZ();

    int nx = wnx;
    int nz = wnz;
    int size = (wnx - 2 * HALF_LENGTH_) * (wnz - 2 * HALF_LENGTH_);

    int flops_per_velocity = 6 * HALF_LENGTH_ + 4;
    int num_of_arrays_velocity = 6;

    ElasticTimer timer("ComputationKernel::ComputeVelocity",
                       size,
                       num_of_arrays_velocity,
                       true,
                       flops_per_velocity);
    timer.Start();

    if (Backend::GetInstance()->GetAlgorithm() == SYCL_ALGORITHM::CPU) {

        Backend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
            size_t num_groups = Backend::GetInstance()->GetWorkgroupNumber();
            size_t wgsize = Backend::GetInstance()->GetWorkgroupSize();

            int compute_nz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetComputationAxisSize();
            int compute_nx = mpParameters->GetHalfLength() +
                             this->mpGridBox->GetWindowAxis()->GetXAxis().GetComputationAxisSize();
            size_t z_stride = (compute_nz + num_groups - 1) / num_groups;
            auto global_range = range<1>(num_groups);
            auto local_range = range<1>(wgsize);
            const float *current = curr_base;
            float *vel_x = particle_vel_x;
            float *vel_z = particle_vel_z;
            const float *den = den_base;
            const size_t wnx = nx;
            const size_t wnz = nz;
            float *coeff_x = mpCoeff->GetHostPointer();
            float *coeff_z = mpCoeff->GetHostPointer();
            float *vel = vel_base;

            cgh.parallel_for_work_group(global_range, local_range, [=](group<1> grp) {
                size_t z_id = grp.get_id(0) * z_stride + HALF_LENGTH_;
                size_t end_z =
                        (z_id + z_stride) < (compute_nz + HALF_LENGTH_) ?
                        (z_id + z_stride) : (compute_nz + HALF_LENGTH_);
                grp.parallel_for_work_item([&](h_item<1> it) {

                    for (size_t iz = z_id; iz < end_z; iz++) {
                        size_t offset = iz * wnx + it.get_local_id(0);

                        for (size_t ix = HALF_LENGTH_; ix < compute_nx; ix += wgsize) {
                            size_t idx = offset + ix;
                            float value_x = 0;
                            float value_z = 0;

                            DERIVE_SEQ_AXIS(idx, 1, 0, -, current, coeff_x, value_x)
                            DERIVE_JUMP_AXIS(idx, wnx, 1, 0, -, current, coeff_z, value_z)
                            if constexpr(KERNEL_MODE_ != KERNEL_MODE::INVERSE) {

                                vel_x[idx] = vel_x[idx] - (den[idx] / dx) * value_x;
                                vel_z[idx] = vel_z[idx] - (den[idx] / dz) * value_z;

                            } else {
                                vel_x[idx] = vel_x[idx] + (den[idx] / dx) * value_x;
                                vel_z[idx] = vel_z[idx] + (den[idx] / dz) * value_z;

                            }
                        }
                    }
                });
            });
        });
    } else if (Backend::GetInstance()->GetAlgorithm() == SYCL_ALGORITHM::GPU_SHARED) {
        int compute_nz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetComputationAxisSize();
        int compute_nx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetComputationAxisSize();

        Backend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
            auto global_range = range<2>(compute_nz, compute_nx);
            auto local_range = range<2>(block_z, block_x);
            sycl::nd_range<2> workgroup_range(global_range, local_range);
            const float *current = curr_base;
            const float *den = den_base;
            float *vel = vel_base;
            float *vel_x = particle_vel_x;
            float *vel_z = particle_vel_z;
            const float *coeff = mpCoeff->GetNativePointer();
            const int idx_range = mpParameters->GetBlockZ();
            const int local_nx = mpParameters->GetBlockX() + 2 * HALF_LENGTH_;
            auto localRange_ptr_cur = range<1>(((mpParameters->GetBlockX() + (2 * HALF_LENGTH_)) *
                                                (mpParameters->GetBlockZ() + (2 * HALF_LENGTH_))));

            accessor<float, 1, access::mode::read_write, access::target::local> tab(
                    localRange_ptr_cur, cgh);

            cgh.parallel_for(workgroup_range,
                             [=](nd_item<2> it) {

                                 float *local = tab.get_pointer();
                                 int idx =
                                         it.get_global_id(1) + HALF_LENGTH_ +
                                         (it.get_global_id(0) + HALF_LENGTH_) * nx;
                                 size_t id0 = it.get_local_id(1);
                                 size_t id1 = it.get_local_id(0);
                                 size_t offset = (id0 + HALF_LENGTH_) +
                                                 (id1 + HALF_LENGTH_) * local_nx;

                                 float coeff_x_device[HALF_LENGTH_];
                                 float coeff_z_device[HALF_LENGTH_];
                                 for (int iter = 0; iter < HALF_LENGTH_; iter++) {
                                     coeff_x_device[iter] = coeff[iter];
                                     coeff_z_device[iter] = coeff[iter];
                                 }
                                 bool copyHaloX = false;
                                 bool copyHaloY = false;
                                 const unsigned int items_X = it.get_local_range(1);

                                 local[offset] = current[idx];

                                 if (id0 < HALF_LENGTH_) {
                                     copyHaloX = true;
                                 }
                                 if (id1 < HALF_LENGTH_) {
                                     copyHaloY = true;
                                 }
                                 if (copyHaloX) {
                                     local[offset - HALF_LENGTH_] = current[idx -
                                                                            HALF_LENGTH_];
                                     local[offset + items_X] = current[idx +
                                                                       items_X];
                                 }
                                 if (copyHaloY) {
                                     local[offset -
                                           HALF_LENGTH_ * local_nx] = current[idx -
                                                                              HALF_LENGTH_ *
                                                                              nx];
                                     local[offset + idx_range * local_nx] = current[
                                             idx + idx_range * nx];
                                 }
                                 it.barrier(access::fence_space::local_space);

                                 float value_x = 0;
                                 float value_z = 0;

                                 for (int iter = 0; iter < HALF_LENGTH_; ++iter) {
                                     value_x += coeff_x_device[iter] *
                                                (local[offset + (iter + 1)] -
                                                 local[offset - iter]);
                                     value_z += coeff_z_device[iter] *
                                                (local[offset +
                                                       ((iter + 1) * local_nx)] -
                                                 local[offset - (iter * local_nx)]);
                                 }
                                 if constexpr (KERNEL_MODE_ != KERNEL_MODE::INVERSE) {

                                     vel_x[idx] =
                                             vel_x[idx] - (den[idx] / dx) * value_x;
                                     vel_z[idx] =
                                             vel_z[idx] - (den[idx] / dz) * value_z;

                                 } else {
                                     vel_x[idx] =
                                             vel_x[idx] + (den[idx] / dx) * value_x;
                                     vel_z[idx] =
                                             vel_z[idx] + (den[idx] / dz) * value_z;
                                 }

                             });
        });
    } else if (Backend::GetInstance()->GetAlgorithm() == SYCL_ALGORITHM::GPU_SEMI_SHARED) {
        int compute_nz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetComputationAxisSize() / block_z;
        int compute_nx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetComputationAxisSize();

        Backend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
            auto global_range = range<2>(compute_nz, compute_nx);
            auto local_range = range<2>(1, block_x);
            auto workgroup_range = nd_range<2>(global_range, local_range);
            const float *current = curr_base;
            float *vel_x = particle_vel_x;
            float *vel_z = particle_vel_z;
            const float *den = den_base;
            float *vel = vel_base;
            const int idx_range = block_z;
            const int local_nx = block_x + 2 * HALF_LENGTH_;
            const int local_nz = block_z + 2 * HALF_LENGTH_;
            float *coeff = mpCoeff->GetNativePointer();

            auto localRange_ptr_cur = range<1>(((block_x + (2 * HALF_LENGTH_)) *
                                                (block_z + (2 * HALF_LENGTH_))));

            accessor<float, 1, access::mode::read_write, access::target::local> tab(
                    localRange_ptr_cur, cgh);
            cgh.parallel_for(workgroup_range,
                             [=](nd_item<2> it) {

                                 float *local = tab.get_pointer();
                                 int idx = it.get_global_id(1) + HALF_LENGTH_ +
                                           (it.get_global_id(0) * idx_range +
                                            HALF_LENGTH_) * nx;
                                 size_t id0 = it.get_local_id(1);
                                 size_t offset = (id0 + HALF_LENGTH_) +
                                                 HALF_LENGTH_ * local_nx;
                                 float coeff_x_device[HALF_LENGTH_];
                                 float coeff_z_device[HALF_LENGTH_];

                                 for (unsigned int iter = 0;
                                      iter < HALF_LENGTH_; iter++) {
                                     coeff_x_device[iter] = coeff[iter];
                                     coeff_z_device[iter] = coeff[iter];
                                 }
                                 bool copyHaloX = false;
                                 if (id0 < HALF_LENGTH_)
                                     copyHaloX = true;
                                 const unsigned int items_X = it.get_local_range(1);
                                 int load_offset = offset - HALF_LENGTH_ * local_nx;
                                 int load_idx = idx - HALF_LENGTH_ * nx;

                                 for (int i = 0; i < local_nz; i++) {
                                     local[load_offset] = current[load_idx];
                                     if (copyHaloX) {
                                         local[load_offset -
                                               HALF_LENGTH_] = current[load_idx -
                                                                       HALF_LENGTH_];
                                         local[load_offset + items_X] = current[
                                                 load_idx + items_X];
                                     }
                                     load_idx += nx;
                                     load_offset += local_nx;
                                 }
                                 it.barrier(access::fence_space::local_space);
                                 for (int i = 0; i < idx_range; i++) {
                                     float value_x = 0, value_z = 0;

                                     for (int iter = 0;
                                          iter < HALF_LENGTH_; ++iter) {
                                         value_x += coeff_x_device[iter] *
                                                    (local[offset + (iter + 1)] -
                                                     local[offset - iter]);
                                         value_z += coeff_z_device[iter] *
                                                    (local[offset + ((iter + 1) *
                                                                     local_nx)] -
                                                     local[offset -
                                                           (iter * local_nx)]);
                                     }

                                     if constexpr (KERNEL_MODE_ != KERNEL_MODE::INVERSE) {
                                         vel_x[idx] =
                                                 vel_x[idx] - (den[idx] / dx) * value_x;
                                         vel_z[idx] =
                                                 vel_z[idx] - (den[idx] / dz) * value_z;
                                     } else {
                                         vel_x[idx] =
                                                 vel_x[idx] + (den[idx] / dx) * value_x;
                                         vel_z[idx] =
                                                 vel_z[idx] + (den[idx] / dz) * value_z;
                                     }

                                     idx += nx;
                                     offset += local_nx;
                                 }

                             });
        });
    } else if (Backend::GetInstance()->GetAlgorithm() == SYCL_ALGORITHM::GPU) {
        int compute_nz = mpGridBox->GetWindowAxis()->GetZAxis().GetComputationAxisSize() / mpParameters->GetBlockZ();

        Backend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
            auto global_range = range<2>(compute_nz, mpGridBox->GetWindowAxis()->GetXAxis().GetComputationAxisSize());
            auto local_range = range<2>(1, block_x);
            auto workgroup_range = nd_range<2>(global_range, local_range);
            const float *current = curr_base;
            float *vel_x = particle_vel_x;
            float *vel_z = particle_vel_z;
            const float *den = den_base;
            const int idx_range = block_z;
            const int local_nx = block_x + 2 * HALF_LENGTH_;
            // const int local_nz = block_z + 2 * HALF_LENGTH_;
            const int pad = 0;
            float *coeff = mpCoeff->GetNativePointer();
            const int *v = mpVerticalIdx->GetNativePointer();
            auto localRange_ptr_cur =
                    range<1>((block_x + (2 * HALF_LENGTH_) + pad));

            accessor<float, 1, access::mode::read_write, access::target::local> tab(
                    localRange_ptr_cur, cgh);
            cgh.parallel_for(workgroup_range,
                             [=](nd_item<2> it) {
                                 float *local = tab.get_pointer();
                                 int idx = it.get_global_id(1) + HALF_LENGTH_ +
                                           (it.get_global_id(0) * idx_range +
                                            HALF_LENGTH_) * nx;
                                 int id0 = it.get_local_id(1);
                                 int offset = (id0 + HALF_LENGTH_);
                                 float coeff_x_device[HALF_LENGTH_];
                                 float coeff_z_device[HALF_LENGTH_];
                                 int v_end = v[HALF_LENGTH_ - 1];
                                 float front[HALF_LENGTH_ + 1];
                                 float back[HALF_LENGTH_];
                                 for (unsigned int iter = 0;
                                      iter <= HALF_LENGTH_; iter++) {
                                     front[iter] = current[idx + nx * iter];
                                 }
                                 for (int iter = 1; iter <= HALF_LENGTH_; iter++) {
                                     back[iter - 1] = current[idx - nx * iter];
                                     coeff_x_device[iter - 1] = coeff[iter - 1];
                                     coeff_z_device[iter - 1] = coeff[iter - 1];
                                 }
                                 bool copyHaloX = false;
                                 if (id0 < HALF_LENGTH_)
                                     copyHaloX = true;
                                 const unsigned int items_X = it.get_local_range(1);

                                 for (int i = 0; i < idx_range; i++) {
                                     it.barrier(access::fence_space::local_space);
                                     local[offset] = front[0];
                                     if (copyHaloX) {
                                         local[offset - HALF_LENGTH_] = current[
                                                 idx - HALF_LENGTH_];
                                         local[offset + items_X] = current[idx +
                                                                           items_X];
                                     }
                                     it.barrier(access::fence_space::local_space);

                                     float value_x = 0, value_z = 0;

                                     for (int iter = 0;
                                          iter < HALF_LENGTH_; ++iter) {
                                         value_x += coeff_x_device[iter] *
                                                    (current[idx + (iter + 1)] -
                                                     current[idx - iter]);
                                         value_z += coeff_z_device[iter] *
                                                    (current[idx +
                                                             ((iter + 1) * nx)] -
                                                     current[idx - (iter * nx)]);
                                     }

                                     if constexpr (KERNEL_MODE_ != KERNEL_MODE::INVERSE) {
                                         vel_x[idx] =
                                                 vel_x[idx] - (den[idx] / dx) * value_x;
                                         vel_z[idx] =
                                                 vel_z[idx] - (den[idx] / dz) * value_z;
                                     } else {
                                         vel_x[idx] =
                                                 vel_x[idx] + (den[idx] / dx) * value_x;
                                         vel_z[idx] =
                                                 vel_z[idx] + (den[idx] / dz) * value_z;
                                     }

                                     idx += nx;
                                 }
                             });
        });
    }
    Backend::GetInstance()->GetDeviceQueue()->wait();
    timer.Stop();
}

void StaggeredComputationKernel::PreprocessModel() {

    int nx = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetActualAxisSize();
    int ny = this->mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetActualAxisSize();
    int nz = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetActualAxisSize();

    float dt = this->mpGridBox->GetDT();

    int full_nx = nx;
    int full_nx_nz = nx * nz;

    Backend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {

        auto global_range = range<3>(nx, ny, nz);
        auto local_range = range<3>(1, 1, 1);
        auto global_nd_range = nd_range<3>(global_range, local_range);
        float *velocity_values = this->mpGridBox->Get(PARM | GB_VEL)->GetNativePointer();
        float *density_values = this->mpGridBox->Get(PARM | GB_DEN)->GetNativePointer();

        cgh.parallel_for(global_nd_range, [=](nd_item<3> it) {
            int x = it.get_global_id(0);
            int y = it.get_global_id(1);
            int z = it.get_global_id(2);

            int offset = y * full_nx_nz + z * full_nx + x;
            float value = velocity_values[offset];
            velocity_values[offset] = value * value * dt * density_values[offset];
            if (density_values[offset] != 0) {
                density_values[offset] = dt / density_values[offset];
            }
        });
    });

    Backend::GetInstance()->GetDeviceQueue()->wait();

}
