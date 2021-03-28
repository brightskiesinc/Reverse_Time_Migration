//
// Created by amr-nasr on 11/21/19.
//

#include <operations/components/independents/concrete/computation-kernels/isotropic/StaggeredComputationKernel.hpp>

#include <operations/components/dependents/concrete/memory-handlers/WaveFieldsMemoryHandler.hpp>

#include <timer/Timer.h>

#include <cmath>

using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;


template void StaggeredComputationKernel::Compute<true, O_2>();

template void StaggeredComputationKernel::Compute<true, O_4>();

template void StaggeredComputationKernel::Compute<true, O_8>();

template void StaggeredComputationKernel::Compute<true, O_12>();

template void StaggeredComputationKernel::Compute<true, O_16>();

template void StaggeredComputationKernel::Compute<false, O_2>();

template void StaggeredComputationKernel::Compute<false, O_4>();

template void StaggeredComputationKernel::Compute<false, O_8>();

template void StaggeredComputationKernel::Compute<false, O_12>();

template void StaggeredComputationKernel::Compute<false, O_16>();

template void StaggeredComputationKernel::Compute<true, O_2>();

template void StaggeredComputationKernel::Compute<true, O_4>();

template void StaggeredComputationKernel::Compute<true, O_8>();

template void StaggeredComputationKernel::Compute<true, O_12>();

template void StaggeredComputationKernel::Compute<true, O_16>();

template void StaggeredComputationKernel::Compute<false, O_2>();

template void StaggeredComputationKernel::Compute<false, O_4>();

template void StaggeredComputationKernel::Compute<false, O_8>();

template void StaggeredComputationKernel::Compute<false, O_12>();

template void StaggeredComputationKernel::Compute<false, O_16>();


template<bool IS_FORWARD_, HALF_LENGTH HALF_LENGTH_>
void StaggeredComputationKernel::Compute() {
    /*
     * Read parameters into local variables to be shared.
     */

    float *curr_base = this->mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();
    float *next_base = this->mpGridBox->Get(WAVE | GB_PRSS | NEXT | DIR_Z)->GetNativePointer();

    float *particle_vel_x = this->mpGridBox->Get(WAVE | GB_PRTC | CURR | DIR_X)->GetNativePointer();
    float *particle_vel_z = this->mpGridBox->Get(WAVE | GB_PRTC | CURR | DIR_Z)->GetNativePointer();

    float *vel_base = this->mpGridBox->Get(PARM | WIND | GB_VEL)->GetNativePointer();
    float *den_base = this->mpGridBox->Get(PARM | WIND | GB_DEN)->GetNativePointer();

    int wnx = this->mpGridBox->GetActualWindowSize(X_AXIS);
    int wnz = this->mpGridBox->GetActualWindowSize(Z_AXIS);

    float dx = this->mpGridBox->GetCellDimensions(X_AXIS);
    float dz = this->mpGridBox->GetCellDimensions(Z_AXIS);

    float *coefficients = this->mpParameters->GetFirstDerivativeStaggeredFDCoefficient();

    int block_x = this->mpParameters->GetBlockX();
    int block_z = this->mpParameters->GetBlockZ();

    int nx_end = this->mpGridBox->GetLogicalWindowSize(X_AXIS) - HALF_LENGTH_;
    int nz_end = this->mpGridBox->GetLogicalWindowSize(Z_AXIS) - HALF_LENGTH_;

    int nx = wnx;
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
    for (int i = 0; i < HALF_LENGTH_; i++) {
        coefficients_x[i] = coefficients[i + 1];
        coefficients_z[i] = coefficients[i + 1];
        vertical[i] = (i + 1) * wnx;
    }


    // start the timers for the velocity kernel.
    Timer *timer = Timer::GetInstance();
    // timer->StartTimer("ComputationKernel:velocity kernel");
    timer->StartTimerKernel("ComputationKernel::velocity kernel", size,
                            num_of_arrays_velocity, true,
                            flops_per_velocity);

// Start the computation by creating the threads.
#pragma omp parallel default(shared)
    {
        float *prev, *next, *den, *vel, *vel_x, *vel_z;

/// Three loops for cache blocking.
/// Utilizing the cache to the maximum to speed up computation.
#pragma omp for schedule(static, 1) collapse(2)
        for (int bz = HALF_LENGTH_; bz < nz_end; bz += block_z) {
            for (int bx = HALF_LENGTH_; bx < nx_end; bx += block_x) {
                /// Calculate the endings appropriately
                /// (Handle remainder of the cache blocking loops).
                int ixEnd = fmin(block_x, nx_end - bx);
                int izEnd = fmin(bz + block_z, nz_end);

                /// Loop on the elements in the block.
                for (int iz = bz; iz < izEnd; ++iz) {

                    // Pre-compute in advance the pointer to the start of the current
                    // start point of the processing.
                    int offset = iz * wnx + bx;

                    // Velocity moves with the full nx and nz not the windows ones.
                    prev = curr_base + offset;
                    den = den_base + offset;

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
                        for (int i = 0; i < HALF_LENGTH_; ++i) {
                            // 3 floating point operations
                            value_x += coefficients_x[i] * (prev[ix + (i + 1)] - prev[ix - i]);
                            // 3 floating point operations
                            value_z += coefficients_z[i] *
                                       (prev[ix + (i + 1) * wnx] - prev[ix - (i * wnx)]);
                        }
                        if (IS_FORWARD_) {
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


    // the end of time of particle velocity kernel
    timer->StopTimer("ComputationKernel::velocity kernel");
    timer->StartTimer("BoundaryManager::ApplyBoundary(Velocity)");
    if (this->mpBoundaryManager != nullptr) {
        this->mpBoundaryManager->ApplyBoundary(1);
    }
    timer->StopTimer("BoundaryManager::ApplyBoundary(Velocity)");
    // start the timer of the pressure kernel
    timer->StartTimerKernel("ComputationKernel::pressure kernel", size,
                            num_of_arrays_pressure, true,
                            flops_per_pressure);
#pragma omp parallel default(shared)
    {
        float *curr, *next, *den, *vel, *vel_x, *vel_z;
        // Pressure Calculation
#pragma omp for schedule(static, 1) collapse(2)
        for (int bz = HALF_LENGTH_; bz < nz_end; bz += block_z) {
            for (int bx = HALF_LENGTH_; bx < nx_end; bx += block_x) {
                // Calculate the endings appropriately (Handle remainder of the cache
                // blocking loops).
                int ixEnd = fmin(block_x, nx_end - bx);
                int izEnd = fmin(bz + block_z, nz_end);
                // Loop on the elements in the block.
                for (int iz = bz; iz < izEnd; ++iz) {
                    // Pre-compute in advance the pointer to the start of the current
                    // start point of the processing.
                    int offset = iz * wnx + bx;
                    // Velocity moves with the full nx and nz not the windows ones.
                    curr = curr_base + offset;
                    next = next_base + offset;

                    vel = vel_base + offset;

                    vel_x = particle_vel_x + offset;
                    vel_z = particle_vel_z + offset;
#pragma vector aligned
#pragma vector vecremainder
#pragma omp simd
#pragma ivdep
                    for (int ix = 0; ix < ixEnd; ++ix) {
                        float value_x = 0;
                        float value_z = 0;
                        for (int i = 0; i < HALF_LENGTH_; ++i) {
                            // 3 floating point operations
                            value_x += coefficients_x[i] * (vel_x[ix + i] - vel_x[ix - (i + 1)]);
                            // 3 floating point operations
                            value_z += coefficients_z[i] *
                                       (vel_z[ix + i * nx] - vel_z[ix - ((i + 1) * nx)]);
                        }
                        if (IS_FORWARD_) {
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
    timer->StopTimer("ComputationKernel::pressure kernel");
}
