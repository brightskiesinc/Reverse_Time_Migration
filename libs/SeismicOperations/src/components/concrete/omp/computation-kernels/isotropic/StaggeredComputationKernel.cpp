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

#include <operations/components/independents/concrete/computation-kernels/isotropic/StaggeredComputationKernel.hpp>

#include <cmath>

#include <operations/components/independents/concrete/computation-kernels/BaseComputationHelpers.hpp>
#include <operations/components/dependents/concrete/memory-handlers/WaveFieldsMemoryHandler.hpp>

#include <bs/timer/api/cpp/BSTimer.hpp>

using namespace std;
using namespace bs::timer;
using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;

FORWARD_DECLARE_COMPUTE_TEMPLATE(StaggeredComputationKernel, ComputePressure)

FORWARD_DECLARE_COMPUTE_TEMPLATE(StaggeredComputationKernel, ComputeVelocity)

template<KERNEL_MODE KERNEL_MODE_, bool IS_2D_, HALF_LENGTH HALF_LENGTH_>
void StaggeredComputationKernel::ComputeVelocity() {
    /*
     * Read parameters into local variables to be shared.
     */
    float *curr_base = this->mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();
    float *next_base = this->mpGridBox->Get(WAVE | GB_PRSS | NEXT | DIR_Z)->GetNativePointer();

    float *particle_vel_x = this->mpGridBox->Get(WAVE | GB_PRTC | CURR | DIR_X)->GetNativePointer();
    float *particle_vel_z = this->mpGridBox->Get(WAVE | GB_PRTC | CURR | DIR_Z)->GetNativePointer();

    float *vel_base = this->mpGridBox->Get(PARM | WIND | GB_VEL)->GetNativePointer();
    float *den_base = this->mpGridBox->Get(PARM | WIND | GB_DEN)->GetNativePointer();


    int wnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    int wnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

    float dx = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetCellDimension();
    float dz = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetCellDimension();


    float dt = this->mpGridBox->GetDT();
    float *coefficients = this->mpParameters->GetFirstDerivativeStaggeredFDCoefficient();

    int block_x = this->mpParameters->GetBlockX();
    int block_z = this->mpParameters->GetBlockZ();

    int nx_end = this->mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize() - HALF_LENGTH_;
    int nz_end = this->mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize() - HALF_LENGTH_;

    int wnxnz = wnx * wnz;
    int nx = wnx;
    int nz = wnz;
    int nxnz = nx * nz;
    int size = (wnx - 2 * HALF_LENGTH_) * (wnz - 2 * HALF_LENGTH_);

    /// General note: floating point operations for forward is the same as backward
    /// (calculated below are for forward). number of floating point operations for
    /// the pressure kernel in 2D for the half_length loop:6*k -2(for adding zeros)
    /// =6*K-2 where K is the half_length 5 floating point operations outside the
    /// half_length loop Total = 6*K-2+5 =6*K+3
    int flops_per_pressure = 6 * HALF_LENGTH_ + 3;

    // vel,curr,next,vel_x,vel_z
    int num_of_arrays_pressure = 5;

    /// General note: floating point operations for forward is the same as backward
    /// (calculated below are for forward). number of floating point operations for
    /// the velocity kernel in 2D for the half_length loop:6*k -2(for adding zeros)
    /// =6*K-2 where K is the half_length 6 floating point operations outside the
    /// half_length loop Total = 6*K-2+6 =6*K+4
    int flops_per_velocity = 6 * HALF_LENGTH_ + 4;

    // curr,den,vel_x(load),vel_x(store),vel_z(load),vel_z(store)
    int num_of_arrays_velocity = 6;

    /*
     * Pre-compute the coefficients for each direction.
     */

    float coefficients_x[HALF_LENGTH_];
    float coefficients_z[HALF_LENGTH_];

    int vertical[HALF_LENGTH_];
    int front[HALF_LENGTH_];
    for (int i = 0; i < HALF_LENGTH_; i++) {
        coefficients_x[i] = coefficients[i + 1];
        coefficients_z[i] = coefficients[i + 1];
        vertical[i] = (i + 1) * wnx;
    }


    // start the timers for the velocity kernel.
    ElasticTimer timer("ComputationKernel::Kernel",
                       size,
                       num_of_arrays_velocity,
                       true,
                       flops_per_velocity);
    timer.Start();

// Start the computation by creating the threads.
#pragma omp parallel default(shared)
    {
        float *prev, *next, *den, *vel, *vel_x, *vel_y, *vel_z;

/// Three loops for cache blocking.
/// Utilizing the cache to the maximum to speed up computation.
#pragma omp for schedule(static, 1) collapse(2)
        for (int bz = HALF_LENGTH_; bz < nz_end; bz += block_z) {
            for (int bx = HALF_LENGTH_; bx < nx_end; bx += block_x) {
                /// Calculate the endings appropriately
                /// (Handle remainder of the cache blocking loops).
                int ixEnd = min(block_x, nx_end - bx);
                int izEnd = min(bz + block_z, nz_end);

                /// Loop on the elements in the block.
                for (int iz = bz; iz < izEnd; ++iz) {

                    // Pre-compute in advance the pointer to the start of the current
                    // start point of the processing.
                    int offset = iz * wnx + bx;

                    prev = curr_base + offset;
                    den = den_base + offset;
                    vel = vel_base + offset;

                    vel_x = particle_vel_x + offset;
                    vel_z = particle_vel_z + offset;
#pragma vector aligned
#pragma vector vecremainder
#pragma omp simd
#pragma ivdep
                    for (int ix = 0; ix < ixEnd; ++ix) {
                        float value_x = 0;
                        float value_y = 0;
                        float value_z = 0;
                        if constexpr (KERNEL_MODE_ == KERNEL_MODE::ADJOINT) {
                            DERIVE_SEQ_AXIS(ix, 1, 0, -, prev, coefficients_x, value_x, vel)
                            DERIVE_JUMP_AXIS(ix, wnx, 1, 0, -, prev, coefficients_z, value_z, vel)
                            vel_x[ix] = vel_x[ix] - value_x / dx;
                            // 3 floating point operations
                            vel_z[ix] = vel_z[ix] - value_z / dz;
                        } else {
                            DERIVE_SEQ_AXIS(ix, 1, 0, -, prev, coefficients_x, value_x)
                            DERIVE_JUMP_AXIS(ix, wnx, 1, 0, -, prev, coefficients_z, value_z)
                            if constexpr(KERNEL_MODE_ == KERNEL_MODE::FORWARD) {
                                // 3 floating point operations
                                vel_x[ix] = vel_x[ix] - (den[ix] / dx) * value_x;
                                // 3 floating point operations
                                vel_z[ix] = vel_z[ix] - (den[ix] / dz) * value_z;
                            } else {
                                vel_x[ix] = vel_x[ix] + (den[ix] / dx) * value_x;
                                vel_z[ix] = vel_z[ix] + (den[ix] / dz) * value_z;
                            }
                        }
                    }
                }
            }
        }
    }
    // the end of time of particle velocity kernel
    timer.Stop();
}

template<KERNEL_MODE KERNEL_MODE_, bool IS_2D_, HALF_LENGTH HALF_LENGTH_>
void StaggeredComputationKernel::ComputePressure() {
    /*
     * Read parameters into local variables to be shared.
     */

    float *curr_base = this->mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();
    float *next_base = this->mpGridBox->Get(WAVE | GB_PRSS | NEXT | DIR_Z)->GetNativePointer();

    float *particle_vel_x = this->mpGridBox->Get(WAVE | GB_PRTC | CURR | DIR_X)->GetNativePointer();
    float *particle_vel_z = this->mpGridBox->Get(WAVE | GB_PRTC | CURR | DIR_Z)->GetNativePointer();

    float *vel_base = this->mpGridBox->Get(PARM | WIND | GB_VEL)->GetNativePointer();
    float *den_base = this->mpGridBox->Get(PARM | WIND | GB_DEN)->GetNativePointer();


    int wnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    int wnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

    float dx = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetCellDimension();
    float dz = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetCellDimension();

    float dt = this->mpGridBox->GetDT();
    float *coefficients = this->mpParameters->GetFirstDerivativeStaggeredFDCoefficient();

    int block_x = this->mpParameters->GetBlockX();
    int block_z = this->mpParameters->GetBlockZ();

    int nx_end = this->mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize() - HALF_LENGTH_;
    int nz_end = this->mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize() - HALF_LENGTH_;

    int wnxnz = wnx * wnz;
    int nx = wnx;
    int nz = wnz;
    int nxnz = nx * nz;
    int size = (wnx - 2 * HALF_LENGTH_) * (wnz - 2 * HALF_LENGTH_);

    /// General note: floating point operations for forward is the same as backward
    /// (calculated below are for forward). number of floating point operations for
    /// the pressure kernel in 2D for the half_length loop:6*k -2(for adding zeros)
    /// =6*K-2 where K is the half_length 5 floating point operations outside the
    /// half_length loop Total = 6*K-2+5 =6*K+3
    int flops_per_pressure = 6 * HALF_LENGTH_ + 3;

    // vel,curr,next,vel_x,vel_z
    int num_of_arrays_pressure = 5;


    /*
     * Pre-compute the coefficients for each direction.
     */

    float coefficients_x[HALF_LENGTH_];
    float coefficients_y[HALF_LENGTH_];
    float coefficients_z[HALF_LENGTH_];

    int vertical[HALF_LENGTH_];
    int front[HALF_LENGTH_];
    for (int i = 0; i < HALF_LENGTH_; i++) {
        coefficients_x[i] = coefficients[i + 1];
        coefficients_z[i] = coefficients[i + 1];
        vertical[i] = (i + 1) * wnx;
        if (!IS_2D_) {
            coefficients_y[i] = coefficients[i + 1];
            front[i] = (i + 1) * wnxnz;
        }
    }


    // start the timers for the velocity kernel.
    ElasticTimer timer("ComputationKernel::Kernel",
                       size,
                       num_of_arrays_pressure,
                       true,
                       flops_per_pressure);
    // start the timer of the pressure kernel
    timer.Start();
#pragma omp parallel default(shared)
    {
        float *curr, *next, *den, *vel, *vel_x, *vel_y, *vel_z;
        // Pressure Calculation
#pragma omp for schedule(static, 1) collapse(2)
        for (int bz = HALF_LENGTH_; bz < nz_end; bz += block_z) {
            for (int bx = HALF_LENGTH_; bx < nx_end; bx += block_x) {
                // Calculate the endings appropriately (Handle remainder of the cache
                // blocking loops).
                int ixEnd = min(block_x, nx_end - bx);
                int izEnd = min(bz + block_z, nz_end);
                // Loop on the elements in the block.
                for (int iz = bz; iz < izEnd; ++iz) {
                    // Pre-compute in advance the pointer to the start of the current
                    // start point of the processing.
                    int offset = iz * wnx + bx;
                    curr = curr_base + offset;
                    next = next_base + offset;
                    den = den_base + offset;
                    vel = vel_base + offset;

                    vel_x = particle_vel_x + offset;
                    vel_z = particle_vel_z + offset;
#pragma vector aligned
#pragma vector vecremainder
#pragma omp simd
#pragma ivdep
                    for (int ix = 0; ix < ixEnd; ++ix) {
                        float value_x = 0;
                        float value_y = 0;
                        float value_z = 0;
                        if constexpr (KERNEL_MODE_ == KERNEL_MODE::ADJOINT) {
                            DERIVE_SEQ_AXIS(ix, 0, 1, -, vel_x, coefficients_x, value_x, den)
                            DERIVE_JUMP_AXIS(ix, nx, 0, 1, -, vel_z, coefficients_z, value_z, den)
                            // 5 floating point operations
                            next[ix] =
                                    curr[ix] - ((value_x / dx) + (value_z / dz));

                        } else {
                            DERIVE_SEQ_AXIS(ix, 0, 1, -, vel_x, coefficients_x, value_x)
                            DERIVE_JUMP_AXIS(ix, nx, 0, 1, -, vel_z, coefficients_z, value_z)
                            if constexpr (KERNEL_MODE_ == KERNEL_MODE::FORWARD) {

                                // 5 floating point operations
                                next[ix] =
                                        curr[ix] - vel[ix] * ((value_x / dx) + (value_z / dz));

                            } else {
                                next[ix] =
                                        curr[ix] + vel[ix] * ((value_x / dx) + (value_z / dz));

                            }
                        }
                    }
                }
            }
        }
    }
    timer.Stop();
}

void StaggeredComputationKernel::PreprocessModel() {
    int nx = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetActualAxisSize();
    int nz = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetActualAxisSize();


    float dt = this->mpGridBox->GetDT();
    float dt2 = dt * dt;

    float *velocity_values = this->mpGridBox->Get(PARM | GB_VEL)->GetNativePointer();

    int full_nx = nx;
    int full_nx_nz = nx * nz;
    float *density_values = this->mpGridBox->Get(PARM | GB_DEN)->GetNativePointer();
    /// Preprocess the velocity model by calculating the
    /// dt * c2 * density component of the wave equation.
#pragma omp parallel default(shared)
    {
#pragma omp for schedule(static) collapse(2)
        for (int z = 0; z < nz; ++z) {
            for (int x = 0; x < nx; ++x) {
                float value = velocity_values[z * full_nx + x];
                int offset = z * full_nx + x;
                velocity_values[offset] =
                        value * value * dt * density_values[offset];
                if (density_values[offset] != 0) {
                    density_values[offset] = dt / density_values[offset];
                }
            }
        }
    }
}
