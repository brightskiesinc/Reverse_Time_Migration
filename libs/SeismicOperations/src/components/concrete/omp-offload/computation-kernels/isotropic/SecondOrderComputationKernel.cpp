//
// Created by ingy-mounir .
//

#include "operations/components/independents/concrete/computation-kernels/isotropic/SecondOrderComputationKernel.hpp"

#include "operations/components/dependents/concrete/memory-handlers/WaveFieldsMemoryHandler.hpp"
#include <timer/Timer.h>
#include <memory-manager/MemoryManager.h>
#include <cmath>
#include <cstring>
#include <omp.h>


#define dfma(a, b, c) (a) * (b) + (c)

using namespace std;
using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;



using namespace std;

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

    if (prev_base == nullptr || curr_base == nullptr || next_base == nullptr) { cout << " no pointer " << endl;}

  //  cout << " 25,0,0,0" << endl;

    float* coeff_x = mpCoeffX->GetNativePointer();
    float* coeff_z = mpCoeffZ->GetNativePointer();
    int* vertical_index = mpVerticalIdx->GetNativePointer();

    int v_end = vertical_index[HALF_LENGTH_ - 1];

    int wnx = this->mpGridBox->GetActualWindowSize(X_AXIS);
    int wny = this->mpGridBox->GetActualWindowSize(Y_AXIS);
    int wnz = this->mpGridBox->GetActualWindowSize(Z_AXIS);

    int block_x = this->mpParameters->GetBlockX();
    int block_y = this->mpParameters->GetBlockY();
    int block_z = this->mpParameters->GetBlockZ();



    int nx_end = this->mpGridBox->GetLogicalWindowSize(X_AXIS) - HALF_LENGTH_;
    int ny_end = 1;
    int nz_end = this->mpGridBox->GetLogicalWindowSize(Z_AXIS) - HALF_LENGTH_;

    int size = (wnx - 2 * HALF_LENGTH_) * (wnz - 2 * HALF_LENGTH_);


    /// General note: floating point operations for forward is the same as backward
    /// (calculated below are for forward). number of floating point operations for
    /// the computation kernel in 2D for the half_length loop:6*k,where K is the
    /// half_length 5 floating point operations outside the half_length loop Total
    /// = 6*K+5 =6*K+5
    int flops_per_second = 6 * HALF_LENGTH_ + 5;

    // Operate on the window velocity (numa consistency ensured).
    int y_start = 0;
    if (!IS_2D_) {
        y_start = HALF_LENGTH_;
        ny_end = wny - HALF_LENGTH_;

        /// General note: floating point operations for forward is the same as
        /// backward (calculated below are for forward). number of floating point
        /// operations for the computation kernel in 3D for the half_length loop:9*K
        /// where K is the half_length 5 floating point operations outside the
        /// half_length loop Total = 9*K+5 =9*K+5
        flops_per_second = 9 * HALF_LENGTH_ + 5;
    }

    Timer *timer = Timer::GetInstance();
    timer->StartTimerKernel("ComputationKernel::kernel", size, 4, true,
                            flops_per_second);

    /*
     * Start the computation by creating the threads.
     */
    int device_num = omp_get_default_device();



#pragma omp target is_device_ptr(prev_base, curr_base , next_base, vel_base,coeff_x,coeff_z,vertical_index) device(device_num)
#pragma omp teams distribute collapse(2)                   \
    num_teams( block_z * (wnx/ block_x)) \
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

                            auto gid = ix + (bz * wnx) ;

                            float front[HALF_LENGTH_ + 1];
                            float back[HALF_LENGTH_];

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



                            back[0] = curr_base[gid - wnx ];
                            if (HALF_LENGTH_ > 1) {
                                back[1] = curr_base[gid -  2 * wnx ];
                            }
                            if (HALF_LENGTH_ > 2) {
                                back[2] = curr_base[gid -  3 * wnx ];
                                back[3] = curr_base[gid -  4 * wnx ];
                            }
                            if (HALF_LENGTH_ > 4) {
                                back[4] = curr_base[gid -  5 * wnx ];
                                back[5] = curr_base[gid -  6 * wnx ];
                            }
                            if (HALF_LENGTH_ > 6) {
                                back[6] = curr_base[gid -  7 * wnx ];
                                back[7] = curr_base[gid -  8 * wnx ];
                            }


                            for (auto iz = bz; iz < izEnd ; iz++) {

                                float value = 0;
                    
                                value = dfma(front[0], mCoeffXYZ, value);

                                value = dfma(front[1] + back[0], coeff_z[0], value);

                                value =dfma(curr_base[gid + 1] + curr_base[gid - 1],coeff_x[0], value);
                   
                                if (HALF_LENGTH_ > 1) {

                                    value = dfma(front[2]+ back[1], coeff_z[1], value);

                                    value =dfma(curr_base[gid + 2] + curr_base[gid - 2] ,coeff_x[1], value);

                                }
                                if (HALF_LENGTH_ > 2) {

                                    value = dfma(front[3] + back[2] , coeff_z[2], value);

                                    value = dfma(front[4] + back[3], coeff_z[3], value);

                                    value =dfma(curr_base[gid + 3] + curr_base[gid - 3] ,coeff_x[2], value);


                                    value =dfma(curr_base[gid + 4] + curr_base[gid - 4],coeff_x[3], value);


                                }
                                if (HALF_LENGTH_ > 4) {

                                    value = dfma(front[5] + back[4], coeff_z[4], value);
                                    value = dfma(front[6] + back[5], coeff_z[5], value);

                                    value =dfma(curr_base[gid + 4] + curr_base[gid - 4] ,coeff_x[4], value);
                         
                                    value =dfma(curr_base[gid + 5] + curr_base[gid - 5],coeff_x[5], value);




                                }
                                if (HALF_LENGTH_ > 6) {
                                    
                                    value = dfma(front[7] + back[6], coeff_z[6], value);

                                    value = dfma(front[8] + back[7], coeff_z[7], value);

                                    value =dfma(curr_base[gid + 6] + curr_base[gid - 6] ,coeff_x[6], value);

                                    value =dfma(curr_base[gid + 7] + curr_base[gid - 7],coeff_x[7], value);

                                }




                                next_base[gid] = (2 * front[0]) - prev_base[gid] + (vel_base[gid] * value);


                                gid += wnx;


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

                                front[HALF_LENGTH_] = curr_base[gid + HALF_LENGTH_ * wnx];


                            } 

                        }

                    }
                }
            
        
       }


    timer->StopTimer("ComputationKernel::kernel");
}
;
