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

#include <cstring>
#include <chrono>
#include <fstream>

#include <bs/base/api/cpp/BSBase.hpp>
#include <bs/timer/api/cpp/BSTimer.hpp>

#include <operations/components/independents/concrete/computation-kernels/isotropic/SecondOrderComputationKernel.hpp>
#include <operations/components/independents/concrete/computation-kernels/BaseComputationHelpers.hpp>

using namespace std;
using namespace cl::sycl;
using namespace bs::base::backend;
using namespace bs::base::memory;
using namespace bs::timer;
using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;


FORWARD_DECLARE_COMPUTE_TEMPLATE(SecondOrderComputationKernel, Compute)

template<KERNEL_MODE KERNEL_MODE_, bool IS_2D_, HALF_LENGTH HALF_LENGTH_>
void SecondOrderComputationKernel::Compute() {
    /* Read parameters into local variables to be shared. */
    size_t nx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    size_t nz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

    float *prev_base = mpGridBox->Get(WAVE | GB_PRSS | PREV | DIR_Z)->GetNativePointer();
    float *curr_base = mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();
    float *next_base = mpGridBox->Get(WAVE | GB_PRSS | NEXT | DIR_Z)->GetNativePointer();

    float *vel_base = mpGridBox->Get(PARM | WIND | GB_VEL)->GetNativePointer();

    /* Pre-compute the coefficients for each direction. */
    int hl = HALF_LENGTH_;
    int flops_per_second = 6 * HALF_LENGTH_ + 5;
    int size = (nx - 2 * HALF_LENGTH_) * (nz - 2 * HALF_LENGTH_);

    ofstream myfile;
    myfile.open ("example.txt", fstream::app);

    ElasticTimer timer("ComputationKernel::Kernel", size, 4, true,
                       flops_per_second);

    auto t_start = std::chrono::high_resolution_clock::now();
    timer.Start();
    if (Backend::GetInstance()->GetAlgorithm() == SYCL_ALGORITHM::CPU) {
        Backend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
            size_t num_groups = Backend::GetInstance()->GetWorkgroupNumber();
            size_t wgsize = Backend::GetInstance()->GetWorkgroupSize();

            int compute_nz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetComputationAxisSize();
            int compute_nx = mpParameters->GetHalfLength() +
                             this->mpGridBox->GetWindowAxis()->GetXAxis().GetComputationAxisSize();

            size_t z_stride = (compute_nz + num_groups - 1) / num_groups;

            // auto global_range = range<2>(full_x, full_z);
            // auto local_range = range<2>(full_x, block_z);

            auto global_range = range<1>(num_groups);
            auto local_range = range<1>(wgsize);
            const size_t wnx = nx;
            const size_t wnz = nz;
            const float *current = curr_base;
            const float *prev = prev_base;
            float *next = next_base;
            const float *vel = vel_base;
            const float *c_x = mpCoeffX->GetNativePointer();
            const float *c_z = mpCoeffZ->GetNativePointer();
            const int *v = mpVerticalIdx->GetNativePointer();
            const float c_xyz = mCoeffXYZ;
            cgh.parallel_for_work_group(global_range, local_range, [=](group<1> grp) {
                size_t z_id = grp.get_id(0) * z_stride + HALF_LENGTH_;
                size_t end_z =
                        (z_id + z_stride) < (compute_nz + HALF_LENGTH_) ?
                        (z_id + z_stride) : (compute_nz +
                                             HALF_LENGTH_);
                grp.parallel_for_work_item([&](h_item<1> it) {
                    for (size_t iz = z_id; iz < end_z; iz++) {
                        size_t offset = iz * wnx + it.get_local_id(0);
                        for (size_t ix = HALF_LENGTH_; ix < compute_nx; ix += wgsize) {
                            size_t idx = offset + ix;
                            float value = current[idx] * c_xyz;
                            DERIVE_SEQ_AXIS_EQ_OFF(idx, 1, +, current, c_x, value)
                            DERIVE_ARRAY_AXIS_EQ_OFF(idx, v, +, current, c_z, value)
                            next[idx] = (2 * current[idx]) - prev[idx] + (vel[idx] * value);
                        }
                    }
                });
            });
        });
    } else if (Backend::GetInstance()->GetAlgorithm() == SYCL_ALGORITHM::GPU_SHARED) {
        int compute_nz = mpGridBox->GetWindowAxis()->GetZAxis().GetComputationAxisSize();
        int compute_nx = mpGridBox->GetWindowAxis()->GetXAxis().GetComputationAxisSize();

        Backend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
            auto global_range = range<2>(compute_nz, compute_nx);
            auto local_range = range<2>(mpParameters->GetBlockZ(), mpParameters->GetBlockX());
            sycl::nd_range<2> workgroup_range(global_range, local_range);
            const float *current = curr_base;
            const float *prev = prev_base;
            float *next = next_base;
            const float *vel = vel_base;
            const float *c_x = mpCoeffX->GetNativePointer();
            const float *c_z = mpCoeffZ->GetNativePointer();
            const float c_xyz = mCoeffXYZ;
            const int *v = mpVerticalIdx->GetNativePointer();
            const int idx_range = mpParameters->GetBlockZ();
            const int local_nx = mpParameters->GetBlockX() + 2 * hl;
            auto localRange_ptr_cur = range<1>(((mpParameters->GetBlockX() + (2 * hl)) *
                                                (mpParameters->GetBlockZ() + (2 * hl))));
            /*  Create an accessor for SLM buffer. */
            accessor<float, 1, access::mode::read_write, access::target::local> tab(
                    localRange_ptr_cur, cgh);
            cgh.parallel_for(workgroup_range, [=](nd_item<2> it) {
                float *local = tab.get_pointer();
                int idx =
                        it.get_global_id(1) + hl + (it.get_global_id(0) + hl) * nx;
                size_t id0 = it.get_local_id(1);
                size_t id1 = it.get_local_id(0);
                size_t identifiant = (id0 + hl) + (id1 + hl) * local_nx;
                float c_x_loc[HALF_LENGTH_];
                float c_z_loc[HALF_LENGTH_];
                int v_loc[HALF_LENGTH_];
                /* Set local coefficient. */
                for (unsigned int iter = 0; iter < HALF_LENGTH_; iter++) {
                    c_x_loc[iter] = c_x[iter];
                    c_z_loc[iter] = c_z[iter];
                    v_loc[iter] = (iter + 1) * local_nx;
                }
                bool copyHaloX = false;
                bool copyHaloY = false;
                const unsigned int items_X = it.get_local_range(1);

                /* Set shared memory. */
                local[identifiant] = current[idx];
                if (id0 < HALF_LENGTH_) {
                    copyHaloX = true;
                }
                if (id1 < HALF_LENGTH_) {
                    copyHaloY = true;
                }
                if (copyHaloX) {
                    local[identifiant - HALF_LENGTH_] = current[idx - HALF_LENGTH_];
                    local[identifiant + items_X] = current[idx + items_X];
                }
                if (copyHaloY) {
                    local[identifiant - HALF_LENGTH_ * local_nx] =
                            current[idx - HALF_LENGTH_ * nx];
                    local[identifiant + idx_range * local_nx] =
                            current[idx + idx_range * nx];
                }
                it.barrier(access::fence_space::local_space);
                float value = 0;
                value = fma(local[identifiant], c_xyz, value);
                for (int iter = 1; iter <= HALF_LENGTH_; iter++) {
                    value = fma(local[identifiant - iter], c_x_loc[iter - 1], value);
                    value = fma(local[identifiant + iter], c_x_loc[iter - 1], value);
                }
                for (int iter = 1; iter <= HALF_LENGTH_; iter++) {
                    value = fma(local[identifiant - v_loc[iter - 1]],
                                c_z_loc[iter - 1], value);
                    value = fma(local[identifiant + v_loc[iter - 1]],
                                c_z_loc[iter - 1], value);
                }
                value = fma(vel[idx], value, -prev[idx]);
                value = fma(2.0f, local[identifiant], value);
                next[idx] = value;
            });
        });
    } else if (Backend::GetInstance()->GetAlgorithm() == SYCL_ALGORITHM::GPU_SEMI_SHARED) {
        int compute_nz = mpGridBox->GetWindowAxis()->GetZAxis().GetComputationAxisSize() / mpParameters->GetBlockZ();

        Backend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
            auto global_range = range<2>(compute_nz, mpGridBox->GetWindowAxis()->GetXAxis().GetComputationAxisSize());
            auto local_range = range<2>(1, mpParameters->GetBlockX());
            sycl::nd_range<2> workgroup_range(global_range, local_range);
            const float *current = curr_base;
            const float *prev = prev_base;
            float *next = next_base;
            const float *vel = vel_base;
            const float *c_x = mpCoeffX->GetNativePointer();
            const float *c_z = mpCoeffZ->GetNativePointer();
            const float c_xyz = mCoeffXYZ;
            const int *v = mpVerticalIdx->GetNativePointer();
            const int idx_range = mpParameters->GetBlockZ();
            const int local_nx = mpParameters->GetBlockX() + 2 * hl;
            const int local_nz = mpParameters->GetBlockZ() + 2 * hl;
            auto localRange_ptr_cur = range<1>(((mpParameters->GetBlockX() + (2 * hl)) *
                                                (mpParameters->GetBlockZ() + (2 * hl))));
            //  Create an accessor for SLM buffer
            accessor<float, 1, access::mode::read_write, access::target::local> tab(
                    localRange_ptr_cur, cgh);
            cgh.parallel_for(workgroup_range, [=](nd_item<2> it) {
                float *local = tab.get_pointer();
                int idx = it.get_global_id(1) + hl +
                          (it.get_global_id(0) * idx_range + hl) * nx;
                size_t id0 = it.get_local_id(1);
                size_t identifiant = (id0 + hl) + hl * local_nx;
                float c_x_loc[HALF_LENGTH_];
                float c_z_loc[HALF_LENGTH_];
                int v_loc[HALF_LENGTH_];
                // Set local coeff.
                for (unsigned int iter = 0; iter < HALF_LENGTH_; iter++) {
                    c_x_loc[iter] = c_x[iter];
                    c_z_loc[iter] = c_z[iter];
                    v_loc[iter] = (iter + 1) * local_nx;
                }
                bool copyHaloX = false;
                if (id0 < HALF_LENGTH_)
                    copyHaloX = true;
                const unsigned int items_X = it.get_local_range(1);
                int load_identifiant = identifiant - hl * local_nx;
                int load_idx = idx - hl * nx;
                // Set Shared Memory.
                for (int i = 0; i < local_nz; i++) {
                    local[load_identifiant] = current[load_idx];
                    if (copyHaloX) {
                        local[load_identifiant - HALF_LENGTH_] =
                                current[load_idx - HALF_LENGTH_];
                        local[load_identifiant + items_X] = current[load_idx + items_X];
                    }
                    load_idx += nx;
                    load_identifiant += local_nx;
                }
                it.barrier(access::fence_space::local_space);
                for (int i = 0; i < idx_range; i++) {
                    float value = 0;
                    value = fma(local[identifiant], c_xyz, value);
                    for (int iter = 1; iter <= HALF_LENGTH_; iter++) {
                        value =
                                fma(local[identifiant - iter], c_x_loc[iter - 1], value);
                        value =
                                fma(local[identifiant + iter], c_x_loc[iter - 1], value);
                    }
                    for (int iter = 1; iter <= HALF_LENGTH_; iter++) {
                        value = fma(local[identifiant - v_loc[iter - 1]],
                                    c_z_loc[iter - 1], value);
                        value = fma(local[identifiant + v_loc[iter - 1]],
                                    c_z_loc[iter - 1], value);
                    }
                    value = fma(vel[idx], value, -prev[idx]);
                    value = fma(2.0f, local[identifiant], value);
                    next[idx] = value;
                    idx += nx;
                    identifiant += local_nx;
                }
            });
        });
    } else if (Backend::GetInstance()->GetAlgorithm() == SYCL_ALGORITHM::GPU) {
        int compute_nz = mpGridBox->GetWindowAxis()->GetZAxis().GetComputationAxisSize() / mpParameters->GetBlockZ();

        Backend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
            auto global_range = range<2>(compute_nz, mpGridBox->GetWindowAxis()->GetXAxis().GetComputationAxisSize());
            auto local_range = range<2>(1, mpParameters->GetBlockX());
            sycl::nd_range<2> workgroup_range(global_range, local_range);
            const float *current = curr_base;
            const float *prev = prev_base;
            float *next = next_base;
            const float *vel = vel_base;
            const float *c_x = mpCoeffX->GetNativePointer();
            const float *c_z = mpCoeffZ->GetNativePointer();
            const float c_xyz = mCoeffXYZ;
            const int *v = mpVerticalIdx->GetNativePointer();
            const int idx_range = mpParameters->GetBlockZ();
            const int pad = 0;
            auto localRange_ptr_cur =
                    range<1>((mpParameters->GetBlockX() + (2 * hl) + pad));

            /*  Create an accessor for SLM buffer. */
            accessor<float, 1, access::mode::read_write, access::target::local> tab(
                    localRange_ptr_cur, cgh);
            cgh.parallel_for(workgroup_range, [=](nd_item<2> it) {
                float *local = tab.get_pointer();
                int idx = it.get_global_id(1) + hl +
                          (it.get_global_id(0) * idx_range + hl) * nx;
                int id0 = it.get_local_id(1);
                int identifiant = (id0 + hl);
                float c_x_loc[HALF_LENGTH_];
                float c_z_loc[HALF_LENGTH_];
                int v_end = v[HALF_LENGTH_ - 1];
                float front[HALF_LENGTH_ + 1];
                float back[HALF_LENGTH_];
                for (unsigned int iter = 0; iter <= HALF_LENGTH_; iter++) {
                    front[iter] = current[idx + nx * iter];
                }
                for (int iter = 1; iter <= HALF_LENGTH_; iter++) {
                    back[iter - 1] = current[idx - nx * iter];
                    c_x_loc[iter - 1] = c_x[iter - 1];
                    c_z_loc[iter - 1] = c_z[iter - 1];
                }
                bool copyHaloX = false;
                if (id0 < HALF_LENGTH_)
                    copyHaloX = true;
                const unsigned int items_X = it.get_local_range(1);
                for (int i = 0; i < idx_range; i++) {
                    it.barrier(access::fence_space::local_space);
                    local[identifiant] = front[0];
                    if (copyHaloX) {
                        local[identifiant - HALF_LENGTH_] = current[idx - HALF_LENGTH_];
                        local[identifiant + items_X] = current[idx + items_X];
                    }
                    it.barrier(access::fence_space::local_space);
                    float value = 0;
                    value = fma(local[identifiant], c_xyz, value);
                    for (int iter = 1; iter <= HALF_LENGTH_; iter++) {
                        value =
                                fma(local[identifiant - iter], c_x_loc[iter - 1], value);
                        value =
                                fma(local[identifiant + iter], c_x_loc[iter - 1], value);
                    }
                    for (int iter = 1; iter <= HALF_LENGTH_; iter++) {
                        value = fma(front[iter], c_z_loc[iter - 1], value);
                        value = fma(back[iter - 1], c_z_loc[iter - 1], value);
                    }
                    value = fma(vel[idx], value, -prev[idx]);
                    value = fma(2.0f, local[identifiant], value);
                    next[idx] = value;
                    idx += nx;
                    for (unsigned int iter = HALF_LENGTH_ - 1; iter > 0; iter--) {
                        back[iter] = back[iter - 1];
                    }
                    back[0] = front[0];
                    for (unsigned int iter = 0; iter < HALF_LENGTH_; iter++) {
                        front[iter] = front[iter + 1];
                    }
                    // Only one new data-point read from global memory
                    // in z-dimension (depth)
                    front[HALF_LENGTH_] = current[idx + v_end];
                }
            });
        });
    }
    Backend::GetInstance()->GetDeviceQueue()->wait();
    timer.Stop();

    auto t_end = std::chrono::high_resolution_clock::now();

    double elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end-t_start).count();


    myfile << elapsed_time_ms << "\n";
    myfile.close();
}

void SecondOrderComputationKernel::PreprocessModel() {

    int nx = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetActualAxisSize();
    int nz = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetActualAxisSize();

    float dt2 = mpGridBox->GetDT() * mpGridBox->GetDT();
    /*
     * Preprocess the velocity model by calculating the dt2 * c2 component of
     * the wave equation.
     */
    Backend::GetInstance()->GetDeviceQueue()->submit(
            [&](sycl::handler &cgh) {
                auto global_range = range<2>(nx, nz);
                float *vel_device = mpGridBox->Get(PARM | GB_VEL)->GetNativePointer();

                cgh.parallel_for(global_range, [=](sycl::id<2> idx) {
                    int x = idx[0];
                    int z = idx[1];
                    float value = vel_device[z * nx + x];
                    vel_device[z * nx + x] =
                            value * value * dt2;
                });
            });
    Backend::GetInstance()->GetDeviceQueue()->wait();
}
