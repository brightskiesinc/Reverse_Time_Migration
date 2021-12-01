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
#include <cmath>

#include <bs/timer/api/cpp/BSTimer.hpp>

#include <operations/components/independents/concrete/computation-kernels/isotropic/SecondOrderComputationKernel.hpp>
#include <operations/components/independents/concrete/computation-kernels/BaseComputationHelpers.hpp>
#include <operations/components/dependents/concrete/memory-handlers/WaveFieldsMemoryHandler.hpp>

using namespace std;
using namespace bs::timer;
using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;

FORWARD_DECLARE_COMPUTE_TEMPLATE(SecondOrderComputationKernel, Compute)

template <KERNEL_MODE KERNEL_MODE_, bool IS_2D_, HALF_LENGTH HALF_LENGTH_>
void SecondOrderComputationKernel::Compute()
{
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

    int wnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    int wnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

    int block_x = this->mpParameters->GetBlockX();
    int block_z = this->mpParameters->GetBlockZ();

    int nx_end = this->mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize() - HALF_LENGTH_;
    int nz_end = this->mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize() - HALF_LENGTH_;

    int size = (wnx - 2 * HALF_LENGTH_) * (wnz - 2 * HALF_LENGTH_);

    /// General note: floating point operations for forward is the same as backward
    /// (calculated below are for forward). number of floating point operations for
    /// the computation kernel in 2D for the half_length loop:6*k,where K is the
    /// half_length 5 floating point operations outside the half_length loop Total
    /// = 6*K+5 =6*K+5
    int flops_per_second = 6 * HALF_LENGTH_ + 5;

    ElasticTimer timer("ComputationKernel::Kernel",
                       size, 4, true,
                       flops_per_second);
    /// @todo Redo calculations for operations since adjoint is different than forward.
    timer.Start();

    /*
     * Start the computation by creating the threads.
     */

#pragma omp parallel default(shared)
    {
        float *prev, *curr, *next, *vel;

/// Three loops for cache blocking.
/// Utilizing the cache to the maximum to speed up computation.
#pragma omp for schedule(static, 1) collapse(2)
        for (int bz = HALF_LENGTH_; bz < nz_end; bz += block_z)
        {
            for (int bx = HALF_LENGTH_; bx < nx_end; bx += block_x)
            {
                /// Calculate the endings appropriately
                /// (Handle remainder of the cache blocking loops).
                int ixEnd = min(block_x, nx_end - bx);
                int izEnd = min(bz + block_z, nz_end);

                /// Loop on the elements in the block.
                for (int iz = bz; iz < izEnd; ++iz)
                {
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
                    for (int ix = 0; ix < ixEnd; ++ix)
                    {
                        /// Calculate the finite difference using sequence of fma
                        /// instructions.
                        float value = 0;

                        /*
                                * 1 floating point operation
                                */
                        value = fma(curr[ix], mCoeffXYZ, value);
                        /// Calculate Finite Difference in the x-direction.
                        /// 3 floating point operations
                        DERIVE_SEQ_AXIS_EQ_OFF(ix, 1, +, curr, coeff_x, value)
                        /// Calculate Finite Difference in the z-direction.
                        /// 3 floating point operations
                        DERIVE_ARRAY_AXIS_EQ_OFF(ix, vertical_index, +, curr, coeff_z, value)
                        /// Calculate the next pressure value according to
                        /// the second order acoustic wave equation.
                        /// 4 floating point operations.
                        next[ix] = (2 * curr[ix]) - prev[ix] + (vel[ix] * value);
                    }
                }
            }
        }
    }
    timer.Stop();
}

void SecondOrderComputationKernel::PreprocessModel()
{
    int nx = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetActualAxisSize();
    int nz = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetActualAxisSize();

    float dt = this->mpGridBox->GetDT();
    float dt2 = dt * dt;

    float *velocity_values = this->mpGridBox->Get(PARM | GB_VEL)->GetNativePointer();

    int full_nx = nx;
    /// Preprocess the velocity model by calculating the
    /// dt2 * c2 component of the wave equation.
#pragma omp parallel default(shared)
    {
#pragma omp for schedule(static) collapse(2)
        for (int z = 0; z < nz; ++z)
        {
            for (int x = 0; x < nx; ++x)
            {
                float value = velocity_values[z * full_nx + x];
                velocity_values[z * full_nx + x] =
                    value * value * dt2;
            }
        }
    }
}
