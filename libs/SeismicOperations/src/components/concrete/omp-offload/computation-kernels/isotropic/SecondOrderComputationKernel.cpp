//
// Created by ingy-mounir .
//

#include <operations/components/independents/concrete/computation-kernels/isotropic/SecondOrderComputationKernel.hpp>

#include <operations/components/dependents/concrete/memory-handlers/WaveFieldsMemoryHandler.hpp>

#include <timer/Timer.h>

#include <cmath>
#include <omp.h>

using namespace std;
using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;

using namespace std;

#pragma omp declare target

float dfma(float a, float b, float c) {
    float val = a * b + c;
    return val;
}

#pragma omp end declare target

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

    float *prev_base =
            this->mpGridBox->Get(WAVE | GB_PRSS | PREV | DIR_Z)->GetNativePointer();
    float *curr_base =
            this->mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();
    float *next_base =
            this->mpGridBox->Get(WAVE | GB_PRSS | NEXT | DIR_Z)->GetNativePointer();

    float *vel_base =
            this->mpGridBox->Get(PARM | WIND | GB_VEL)->GetNativePointer();

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

    /// General note: floating point operations for forward is the same as
    /// backward (calculated below are for forward). number of floating point
    /// operations for the computation kernel in 2D for the half_length
    /// loop:6*k,where K is the half_length 5 floating point operations outside
    /// the half_length loop Total = 6*K+5 =6*K+5
    int flops_per_second = 6 * HALF_LENGTH_ + 5;

    Timer *timer = Timer::GetInstance();
    timer->StartTimerKernel("ComputationKernel::kernel", size, 4, true,
                            flops_per_second);

    /*
     * Start the computation by creating the threads.
     */
    int device_num = omp_get_default_device();

#pragma omp target                                                                              \
    is_device_ptr(prev_base, curr_base, next_base, vel_base, coeff_x, coeff_z, vertical_index)  \
    device(device_num)
#pragma omp teams distribute collapse(2)                                                        \
    num_teams((wnz / block_z) * (wnx / block_x))                                                \
    thread_limit(block_x)
    {

        for (int bz = HALF_LENGTH_; bz < nz_end; bz += block_z) {
            for (int bx = HALF_LENGTH_; bx < nx_end; bx += block_x) {
                /// Calculate the endings appropriately
                /// (Handle remainder of the cache blocking loops).
                int ixEnd = fmin(bx + block_x, nx_end);
                int izEnd = fmin(bz + block_z, nz_end);

                /// Loop on the elements in the block.
#pragma omp parallel for simd schedule(static, 1)
                for (auto ix = bx; ix < ixEnd; ix++) {
                    // Pre-compute and advance the pointer to the start of the current
                    // start point of the processing.

                    auto gid = ix + (bz * wnx);

                    float front[HALF_LENGTH_ + 1];
                    float back[HALF_LENGTH_];

                    for (unsigned int iter = 0; iter <= HALF_LENGTH_; iter++) {
                        front[iter] = curr_base[gid + wnx * iter];
                    }
                    for (unsigned int iter = 1; iter <= HALF_LENGTH_; iter++) {
                        back[iter - 1] = curr_base[gid - wnx * iter];
                    }

                    for (auto iz = bz; iz < izEnd; iz++) {
                        float value = 0;
                        value = dfma(front[0], mCoeffXYZ, value);

                        for (int iter = 1; iter <= HALF_LENGTH_; iter++) {
                            value = dfma(front[iter], coeff_z[iter - 1], value);
                            value = dfma(back[iter - 1], coeff_z[iter - 1], value);
                        }
                        for (int iter = 1; iter <= HALF_LENGTH_; iter++) {
                            value = dfma(curr_base[gid + iter], coeff_x[iter - 1], value);
                            value = dfma(curr_base[gid - iter], coeff_x[iter - 1], value);
                        }

                        next_base[gid] = (2 * front[0]) - prev_base[gid] + (vel_base[gid] * value);
                        gid += wnx;

                        for (unsigned int iter = HALF_LENGTH_ - 1; iter > 0; iter--) {
                            back[iter] = back[iter - 1];
                        }
                        back[0] = front[0];
                        for (unsigned int iter = 0; iter < HALF_LENGTH_; iter++) {
                            front[iter] = front[iter + 1];
                        }
                        front[HALF_LENGTH_] = curr_base[gid + HALF_LENGTH_ * wnx];
                    }
                }
            }
        }
    }
    timer->StopTimer("ComputationKernel::kernel");
}
