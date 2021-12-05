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

#include "operations/components/independents/concrete/computation-kernels/isotropic/StaggeredComputationKernel.hpp"
#include "operations/components/dependents/concrete/memory-handlers/WaveFieldsMemoryHandler.hpp"
#include <operations/data-units/concrete/holders/FrameBuffer.hpp>
#include <omp.h>
#include <bs/timer/api/cpp/BSTimer.hpp>
#include <operations/components/independents/concrete/computation-kernels/BaseComputationHelpers.hpp>


using namespace std;
using namespace bs::timer;
using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;
using namespace bs::base::exceptions;


FORWARD_DECLARE_COMPUTE_TEMPLATE(StaggeredComputationKernel, ComputePressure)

FORWARD_DECLARE_COMPUTE_TEMPLATE(StaggeredComputationKernel, ComputeVelocity)

template<KERNEL_MODE KERNEL_MODE_, bool IS_2D_, HALF_LENGTH HALF_LENGTH_>
void
StaggeredComputationKernel::ComputeVelocity() {

    float *curr_base = this->mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();
    float *den_base = this->mpGridBox->Get(PARM | WIND | GB_DEN)->GetNativePointer();
    float *vel_base = this->mpGridBox->Get(PARM | WIND | GB_VEL)->GetNativePointer();
    float *particle_vel_x = this->mpGridBox->Get(WAVE | GB_PRTC | CURR | DIR_X)->GetNativePointer();
    float *particle_vel_z = this->mpGridBox->Get(WAVE | GB_PRTC | CURR | DIR_Z)->GetNativePointer();

    if (is_null_ptr(curr_base) || is_null_ptr(den_base) || is_null_ptr(particle_vel_x) || is_null_ptr(particle_vel_z)) {
        throw NULL_POINTER_EXCEPTION();
    }

    float *coeff_x = mpCoeff->GetNativePointer();
    float *coeff_z = mpCoeff->GetNativePointer();

    int wnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    int wnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

    float dx = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetCellDimension();
    float dz = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetCellDimension();

    int block_x = this->mpParameters->GetBlockX();
    int block_z = this->mpParameters->GetBlockZ();

    int nx_end = this->mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize() - HALF_LENGTH_;
    int nz_end = this->mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize() - HALF_LENGTH_;

    int size = (wnx - 2 * HALF_LENGTH_) * (wnz - 2 * HALF_LENGTH_);
    int flops_per_velocity = 6 * HALF_LENGTH_ + 4;
    int num_of_arrays_velocity = 6;

    ElasticTimer timer("ComputationKernel::ComputeVelocity",
                       size, num_of_arrays_velocity, true,
                       flops_per_velocity);
    timer.Start();

    int device_num = omp_get_default_device();
#pragma omp target is_device_ptr(curr_base, den_base, vel_base, particle_vel_x, particle_vel_z, coeff_x, coeff_z) device(device_num)
#pragma omp teams distribute collapse(2)                   \
    num_teams((block_z) * (wnx/ block_x)) \
        thread_limit(block_x)
    {
        for (int bz = HALF_LENGTH_; bz < nz_end; bz += block_z) {
            for (int bx = HALF_LENGTH_; bx < nx_end; bx += block_x) {

                int ixEnd = min(bx + block_x, nx_end);
                int izEnd = min(bz + block_z, nz_end);


#pragma omp parallel for simd schedule(static, 1)
                for (auto ix = bx; ix < ixEnd; ix++) {

                    // Pre-compute and advance the pointer to the start of the current
                    // start point of the processing.
                    auto gid = ix + (bz * wnx);

                    for (auto iz = bz; iz < izEnd; iz++) {
                        float value_x = 0, value_z = 0;

                        DERIVE_SEQ_AXIS(gid, 1, 0, -, curr_base, coeff_x, value_x)
                        DERIVE_JUMP_AXIS(gid, wnx, 1, 0, -, curr_base, coeff_z, value_z)

                        if constexpr(KERNEL_MODE_ != KERNEL_MODE::INVERSE) {

                            particle_vel_x[gid] = particle_vel_x[gid] - (den_base[gid] / dx) * value_x;
                            particle_vel_z[gid] = particle_vel_z[gid] - (den_base[gid] / dz) * value_z;
                        } else {

                            particle_vel_x[gid] = particle_vel_x[gid] + (den_base[gid] / dx) * value_x;
                            particle_vel_z[gid] = particle_vel_z[gid] + (den_base[gid] / dz) * value_z;

                        }

                        gid += wnx;
                    }
                }
            }
        }
    }
    timer.Stop();
}

template<KERNEL_MODE KERNEL_MODE_, bool IS_2D_, HALF_LENGTH HALF_LENGTH_>
void
StaggeredComputationKernel::ComputePressure() {

    /*
    * Read parameters into local variables to be shared.
    */
    float *curr_base = this->mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();
    float *next_base = this->mpGridBox->Get(WAVE | GB_PRSS | NEXT | DIR_Z)->GetNativePointer();

    float *particle_vel_x = this->mpGridBox->Get(WAVE | GB_PRTC | CURR | DIR_X)->GetNativePointer();
    float *particle_vel_z = this->mpGridBox->Get(WAVE | GB_PRTC | CURR | DIR_Z)->GetNativePointer();

    float *vel_base = this->mpGridBox->Get(PARM | WIND | GB_VEL)->GetNativePointer();
    float *den_base = this->mpGridBox->Get(PARM | WIND | GB_DEN)->GetNativePointer();

    if (is_null_ptr(curr_base) || is_null_ptr(next_base) || is_null_ptr(vel_base) || is_null_ptr(particle_vel_x) ||
        is_null_ptr(particle_vel_z) || is_null_ptr(den_base)) {
        throw NULL_POINTER_EXCEPTION();
    }

    int wnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    int wnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

    int block_x = this->mpParameters->GetBlockX();
    int block_z = this->mpParameters->GetBlockZ();

    float dx = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetCellDimension();
    float dz = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetCellDimension();

    int nx_end = this->mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize() - HALF_LENGTH_;
    int nz_end = this->mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize() - HALF_LENGTH_;

    int size = (wnx - 2 * HALF_LENGTH_) * (wnz - 2 * HALF_LENGTH_);
    int flops_per_pressure = 6 * HALF_LENGTH_ + 3;
    int num_of_arrays_pressure = 5;

    float *coeff_x = mpCoeff->GetNativePointer();
    float *coeff_z = mpCoeff->GetNativePointer();

    ElasticTimer timer("ComputationKernel::ComputePressure",
                       size, num_of_arrays_pressure, true,
                       flops_per_pressure);
    timer.Start();


    int device_num = omp_get_default_device();

#pragma omp target is_device_ptr(curr_base, next_base, den_base, particle_vel_x, particle_vel_z, vel_base, coeff_x, coeff_z) device(device_num)
#pragma omp teams distribute collapse(2)                   \
    num_teams((block_z) * (wnx/ block_x)) \
        thread_limit(block_x)
    {
        for (int bz = HALF_LENGTH_; bz < nz_end; bz += block_z) {
            for (int bx = HALF_LENGTH_; bx < nx_end; bx += block_x) {

                int ixEnd = min(bx + block_x, nx_end);
                int izEnd = min(bz + block_z, nz_end);

#pragma omp parallel for simd schedule(static, 1)

                for (auto ix = bx; ix < ixEnd; ix++) {

                    auto gid = ix + (bz * wnx);

                    for (auto iz = bz; iz < izEnd; iz++) {

                        float value_x = 0, value_z = 0;

                        DERIVE_SEQ_AXIS(gid, 0, 1, -, particle_vel_x, coeff_x, value_x)
                        DERIVE_JUMP_AXIS(gid, wnx, 0, 1, -, particle_vel_z, coeff_z, value_z)

                        if constexpr (KERNEL_MODE_ != KERNEL_MODE::INVERSE) {
                            next_base[gid] = curr_base[gid] - vel_base[gid] * ((value_x / dx) + (value_z / dz));
                        } else {
                            next_base[gid] = curr_base[gid] + vel_base[gid] * ((value_x / dx) + (value_z / dz));
                        }

                        gid += wnx;
                    }
                }
            }
        }
    }
    timer.Stop();
}

void
StaggeredComputationKernel::PreprocessModel() {

    int nx = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetActualAxisSize();
    int nz = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetActualAxisSize();

    float dt = this->mpGridBox->GetDT();

    float *velocity_values = this->mpGridBox->Get(PARM | GB_VEL)->GetNativePointer();
    float *density_values = this->mpGridBox->Get(PARM | GB_DEN)->GetNativePointer();

    int full_nx = nx;

    int device_num = omp_get_default_device();
#pragma omp target is_device_ptr(velocity_values, density_values) device(device_num)
#pragma omp teams distribute for schedule(static) collapse(2)
    for (int z = 0; z < nz; ++z) {
        for (int x = 0; x < nx; ++x) {
            float value = velocity_values[z * full_nx + x];
            int offset = z * full_nx + x;
            velocity_values[offset] = value * value * dt * density_values[offset];
            if (density_values[offset] != 0) {
                density_values[offset] = dt / density_values[offset];
            }
        }
    }
}
