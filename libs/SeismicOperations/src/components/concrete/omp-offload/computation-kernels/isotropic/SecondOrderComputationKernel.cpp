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

#include <omp.h>

#include <bs/timer/api/cpp/BSTimer.hpp>

#include <operations/components/independents/concrete/computation-kernels/isotropic/SecondOrderComputationKernel.hpp>
#include <operations/components/dependents/concrete/memory-handlers/WaveFieldsMemoryHandler.hpp>
#include <operations/components/independents/concrete/computation-kernels/BaseComputationHelpers.hpp>

using namespace std;
using namespace bs::base::exceptions;
using namespace bs::timer;
using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;

FORWARD_DECLARE_COMPUTE_TEMPLATE(SecondOrderComputationKernel, Compute)


template<KERNEL_MODE KERNEL_MODE_, bool IS_2D_, HALF_LENGTH HALF_LENGTH_>
void
SecondOrderComputationKernel::Compute() {
    /*
     * Read parameters into local variables to be shared.
     */

    float *prev_base = this->mpGridBox->Get(WAVE | GB_PRSS | PREV | DIR_Z)->GetNativePointer();
    float *curr_base = this->mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();
    float *next_base = this->mpGridBox->Get(WAVE | GB_PRSS | NEXT | DIR_Z)->GetNativePointer();

    float *vel_base = this->mpGridBox->Get(PARM | WIND | GB_VEL)->GetNativePointer();


    if (is_null_ptr(prev_base) || is_null_ptr(curr_base) || is_null_ptr(next_base) || is_null_ptr(vel_base)) {
        throw NULL_POINTER_EXCEPTION();
    }

    float *coeff_x = mpCoeffX->GetNativePointer();
    float *coeff_z = mpCoeffZ->GetNativePointer();


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

    //finding the gpu device
    int device_num = omp_get_default_device();



// all the arrays are now on the device , checked to be all present and then we use teams to make all 
// the EU work on the arrays and then distribute for  making all threads in the EU execute the shared part of the loop 
// we  control the number of teams and thread per team
#pragma omp target is_device_ptr(prev_base, curr_base, next_base, vel_base, coeff_x, coeff_z) device(device_num)
#pragma omp teams distribute collapse(2)                   \
    num_teams((block_z) * (wnx/ block_x)) \
        thread_limit(block_x)
    {

        for (int bz = HALF_LENGTH_; bz < nz_end; bz += block_z) {
            for (int bx = HALF_LENGTH_; bx < nx_end; bx += block_x) {
                /// Calculate the endings appropriately
                /// (Handle remainder of the cache blocking loops).
                int ixEnd = min(bx + block_x, nx_end);
                int izEnd = min(bz + block_z, nz_end);

                /// Loop on the elements in the block.
#pragma omp parallel for simd schedule(static, 1)
                for (auto ix = bx; ix < ixEnd; ix++) {
                    // Pre-compute and advance the pointer to the start of the current
                    // start point of the processing.

                    auto gid = ix + (bz * wnx);
// the front and back arrays are temp arrays where you store the elements above and below the current element in the x row 
// the first loop is at the x direction and the inner loop is at the z direction                            

                    float front[HALF_LENGTH_ + 1];
                    float back[HALF_LENGTH_];

// set the values for the front array , it is a (hl +1) size 
// the front also have the current element of x  at 0 index            
                    front[0] = curr_base[gid];
                    front[1] = curr_base[gid + wnx * 1];

                    if (HALF_LENGTH_ > 1) {
                        front[2] = curr_base[gid + wnx * 2];
                    }
                    if (HALF_LENGTH_ > 2) {
                        front[3] = curr_base[gid + wnx * 3];
                        front[4] = curr_base[gid + wnx * 4];
                    }

                    if (HALF_LENGTH_ > 4) {
                        front[5] = curr_base[gid + wnx * 5];
                        front[6] = curr_base[gid + wnx * 6];
                    }

                    if (HALF_LENGTH_ > 6) {
                        front[7] = curr_base[gid + wnx * 7];
                        front[8] = curr_base[gid + wnx * 8];
                    }


// set the values for the back arrays 
                    back[0] = curr_base[gid - wnx];
                    if (HALF_LENGTH_ > 1) {
                        back[1] = curr_base[gid - 2 * wnx];
                    }
                    if (HALF_LENGTH_ > 2) {
                        back[2] = curr_base[gid - 3 * wnx];
                        back[3] = curr_base[gid - 4 * wnx];
                    }
                    if (HALF_LENGTH_ > 4) {
                        back[4] = curr_base[gid - 5 * wnx];
                        back[5] = curr_base[gid - 6 * wnx];
                    }
                    if (HALF_LENGTH_ > 6) {
                        back[6] = curr_base[gid - 7 * wnx];
                        back[7] = curr_base[gid - 8 * wnx];
                    }

// applying the computation kernel on the current element 
// for the vertical direction we use the front and back elements                            
                    for (auto iz = bz; iz < izEnd; iz++) {

                        float value = 0;
                        value = fma(front[0], mCoeffXYZ, value);

                        value = fma(front[1] + back[0], coeff_z[0], value);


                        if (HALF_LENGTH_ > 1) {

                            value = fma(front[2] + back[1], coeff_z[1], value);


                        }
                        if (HALF_LENGTH_ > 2) {

                            value = fma(front[3] + back[2], coeff_z[2], value);

                            value = fma(front[4] + back[3], coeff_z[3], value);

                        }
                        if (HALF_LENGTH_ > 4) {

                            value = fma(front[5] + back[4], coeff_z[4], value);
                            value = fma(front[6] + back[5], coeff_z[5], value);

                        }
                        if (HALF_LENGTH_ > 6) {

                            value = fma(front[7] + back[6], coeff_z[6], value);

                            value = fma(front[8] + back[7], coeff_z[7], value);

                        }
// use this macro to continue the computation for the horizontal elements 
                        DERIVE_SEQ_AXIS_EQ_OFF(gid, 1, +, curr_base, coeff_x, value)

// adding the calculated value 
                        next_base[gid] = (2 * front[0]) - prev_base[gid] + (vel_base[gid] * value);

// updating the pointer for the next cell , we move one row down 
                        gid += wnx;

// updating the values of back and front arrays by a shift of one element down vertically 

                        if (HALF_LENGTH_ > 6) {
                            back[7] = back[6];
                            back[6] = back[5];
                        }
                        if (HALF_LENGTH_ > 4) {
                            back[5] = back[4];
                            back[4] = back[3];
                        }
                        if (HALF_LENGTH_ > 2) {
                            back[3] = back[2];
                            back[2] = back[1];
                        }
                        if (HALF_LENGTH_ > 1) {
                            back[1] = back[0];
                        }
                        back[0] = front[0];


                        front[0] = front[1];
                        if (HALF_LENGTH_ > 1) {
                            front[1] = front[2];
                        }
                        if (HALF_LENGTH_ > 2) {
                            front[2] = front[3];
                            front[3] = front[4];
                        }

                        if (HALF_LENGTH_ > 4) {
                            front[4] = front[5];
                            front[5] = front[6];
                        }

                        if (HALF_LENGTH_ > 6) {
                            front[6] = front[7];
                            front[7] = front[8];
                        }

// updating the last element of front to have the value of the new current element 
                        front[HALF_LENGTH_] = curr_base[gid + HALF_LENGTH_ * wnx];

                    }

                }

            }
        }
    }
    timer.Stop();
}

void
SecondOrderComputationKernel::PreprocessModel() {

    int nx = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetActualAxisSize();
    int ny = this->mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetActualAxisSize();
    int nz = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetActualAxisSize();

    float dt = this->mpGridBox->GetDT();
    float dt2 = dt * dt;

    float *velocity_values = this->mpGridBox->Get(PARM | GB_VEL)->GetNativePointer();

    int full_nx = nx;
    int full_nx_nz = nx * nz;


    // finding the gpu device
    int device_num = omp_get_default_device();


    /// Preprocess the velocity model by calculating the
    /// dt2 * c2 component of the wave equation.

#pragma omp target is_device_ptr(velocity_values) device(device_num)
#pragma omp teams distribute parallel for collapse(2) schedule(static, 1)

    for (int z = 0; z < nz; ++z) {
        for (int x = 0; x < nx; ++x) {
            float value = velocity_values[z * full_nx + x];
            velocity_values[z * full_nx + x] =
                    value * value * dt2;
        }
    }
}
