//
// Created by amr-nasr on 11/21/19.
//

#include <operations/components/independents/concrete/computation-kernels/isotropic/SecondOrderComputationKernel.hpp>

#include <operations/components/dependents/concrete/memory-handlers/WaveFieldsMemoryHandler.hpp>

#include <timer/Timer.h>

#include <cmath>

#define fma(a, b, c) (a) * (b) + (c)

using namespace std;
using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;


template void SecondOrderComputationKernel::Compute<true, O_2>();

template void SecondOrderComputationKernel::Compute<true, O_4>();

template void SecondOrderComputationKernel::Compute<true, O_8>();

template void SecondOrderComputationKernel::Compute<true, O_12>();

template void SecondOrderComputationKernel::Compute<true, O_16>();

template void SecondOrderComputationKernel::Compute<false, O_2>();

template void SecondOrderComputationKernel::Compute<false, O_4>();

template void SecondOrderComputationKernel::Compute<false, O_8>();

template void SecondOrderComputationKernel::Compute<false, O_12>();

template void SecondOrderComputationKernel::Compute<false, O_16>();

template<bool IS_2D_, HALF_LENGTH HALF_LENGTH_>
void SecondOrderComputationKernel::Compute() {
    /*
     * Read parameters into local variables to be shared.
     */

    float *prev_base = this->mpGridBox->Get(WAVE | GB_PRSS | PREV | DIR_Z)->GetNativePointer();
    float *curr_base = this->mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();
    float *next_base = this->mpGridBox->Get(WAVE | GB_PRSS | NEXT | DIR_Z)->GetNativePointer();

    float *vel_base = this->mpGridBox->Get(PARM | WIND | GB_VEL)->GetNativePointer();

    float *coeff_x = mpCoeffX->GetNativePointer();
    float *coeff_z = mpCoeffZ->GetNativePointer();
    int *vertical_index = mpVerticalIdx->GetNativePointer();

    int wnx = this->mpGridBox->GetActualWindowSize(X_AXIS);
    int wnz = this->mpGridBox->GetActualWindowSize(Z_AXIS);

    int block_x = this->mpParameters->GetBlockX();
    int block_z = this->mpParameters->GetBlockZ();

    int nx_end = this->mpGridBox->GetLogicalWindowSize(X_AXIS) - HALF_LENGTH_;
    int nz_end = this->mpGridBox->GetLogicalWindowSize(Z_AXIS) - HALF_LENGTH_;

    int size = (wnx - 2 * HALF_LENGTH_) * (wnz - 2 * HALF_LENGTH_);

    /// General note: floating point operations for forward is the same as backward
    /// (calculated below are for forward). number of floating point operations for
    /// the computation kernel in 2D for the half_length loop:6*k,where K is the
    /// half_length 5 floating point operations outside the half_length loop Total
    /// = 6*K+5 =6*K+5
    int flops_per_second = 6 * HALF_LENGTH_ + 5;

    Timer *timer = Timer::GetInstance();
    timer->StartTimerKernel("ComputationKernel::kernel", size, 4, true,
                            flops_per_second);

    /*
     * Start the computation by creating the threads.
     */

#pragma omp parallel default(shared)
    {
        float *prev, *curr, *next, *vel;

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
                    // Pre-compute and advance the pointer to the start of the current
                    // start point of the processing.
                    int offset = iz * wnx + bx;

                    // Velocity moves with the full nx and nz not the windows ones.
                    prev = prev_base + offset;
                    curr = curr_base + offset;
                    next = next_base + offset;

                    vel = vel_base + offset;

#pragma vector aligned
#pragma vector vecremainder
#pragma omp simd
#pragma ivdep
                    for (int ix = 0; ix < ixEnd; ++ix) {
                        /// Calculate the finite difference using sequence of fma
                        /// instructions.
                        float value = 0;

                        /*
                         * 1 floating point operation
                         */

                        value = fma(curr[ix], mCoeffXYZ, value);
                        /// Calculate Finite Difference in the x-direction.
                        /// 3 floating point operations
                        value = fma(curr[ix - 1] + curr[ix + 1], coeff_x[0], value);
                        if (HALF_LENGTH_ > 1) {
                            value = fma(curr[ix - 2] + curr[ix + 2], coeff_x[1], value);
                        }
                        if (HALF_LENGTH_ > 2) {
                            value = fma(curr[ix - 3] + curr[ix + 3], coeff_x[2], value);
                            value = fma(curr[ix - 4] + curr[ix + 4], coeff_x[3], value);
                        }
                        if (HALF_LENGTH_ > 4) {
                            value = fma(curr[ix - 5] + curr[ix + 5], coeff_x[4], value);
                            value = fma(curr[ix - 6] + curr[ix + 6], coeff_x[5], value);
                        }
                        if (HALF_LENGTH_ > 6) {
                            value = fma(curr[ix - 7] + curr[ix + 7], coeff_x[6], value);
                            value = fma(curr[ix - 8] + curr[ix + 8], coeff_x[7], value);
                        }
                        /// Calculate Finite Difference in the z-direction.
                        /// 3 floating point operations
                        value = fma(curr[ix - vertical_index[0]] + curr[ix + vertical_index[0]],
                                    coeff_z[0], value);
                        if (HALF_LENGTH_ > 1) {
                            value = fma(curr[ix - vertical_index[1]] + curr[ix + vertical_index[1]],
                                        coeff_z[1], value);
                        }
                        if (HALF_LENGTH_ > 2) {
                            value = fma(curr[ix - vertical_index[2]] + curr[ix + vertical_index[2]],
                                        coeff_z[2], value);
                            value = fma(curr[ix - vertical_index[3]] + curr[ix + vertical_index[3]],
                                        coeff_z[3], value);
                        }
                        if (HALF_LENGTH_ > 4) {
                            value = fma(curr[ix - vertical_index[4]] + curr[ix + vertical_index[4]],
                                        coeff_z[4], value);
                            value = fma(curr[ix - vertical_index[5]] + curr[ix + vertical_index[5]],
                                        coeff_z[5], value);
                        }
                        if (HALF_LENGTH_ > 6) {
                            value = fma(curr[ix - vertical_index[6]] + curr[ix + vertical_index[6]],
                                        coeff_z[6], value);
                            value = fma(curr[ix - vertical_index[7]] + curr[ix + vertical_index[7]],
                                        coeff_z[7], value);
                        }
                        /// Calculate the next pressure value according to
                        /// the second order acoustic wave equation.
                        /// 4 floating point operations.
                        next[ix] = (2 * curr[ix]) - prev[ix] + (vel[ix] * value);
                    }
                }
            }
        }
    }
    timer->StopTimer("ComputationKernel::kernel");
}
