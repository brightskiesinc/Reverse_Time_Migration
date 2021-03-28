//
// Created by amr-nasr on 18/11/2019.
//

#include <operations/components/independents/concrete/boundary-managers/CPMLBoundaryManager.hpp>

#include <operations/components/independents/concrete/boundary-managers/extensions/HomogenousExtension.hpp>
#include <timer/Timer.h>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>

#ifndef PWR2
#define PWR2(EXP) ((EXP) * (EXP))
#endif

#define fma(a, b, c) (((a) * (b)) + (c))

using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;

template void CPMLBoundaryManager::FillCPMLCoefficients<1>();

template void CPMLBoundaryManager::FillCPMLCoefficients<2>();

template void CPMLBoundaryManager::FillCPMLCoefficients<3>();

template void CPMLBoundaryManager::ApplyAllCPML<O_2>();

template void CPMLBoundaryManager::ApplyAllCPML<O_4>();

template void CPMLBoundaryManager::ApplyAllCPML<O_8>();

template void CPMLBoundaryManager::ApplyAllCPML<O_12>();

template void CPMLBoundaryManager::ApplyAllCPML<O_16>();

template<int HALF_LENGTH_>
void CPMLBoundaryManager::ApplyAllCPML() {
    CalculateFirstAuxiliary<1, true, HALF_LENGTH_>();
    CalculateFirstAuxiliary<2, true, HALF_LENGTH_>();
    CalculateFirstAuxiliary<1, false, HALF_LENGTH_>();
    CalculateFirstAuxiliary<2, false, HALF_LENGTH_>();

    CalculateCPMLValue<1, true, HALF_LENGTH_>();
    CalculateCPMLValue<2, true, HALF_LENGTH_>();
    CalculateCPMLValue<1, false, HALF_LENGTH_>();
    CalculateCPMLValue<2, false, HALF_LENGTH_>();
}

// make them private to the class , extend to th 3d
template<int DIRECTION_>
void CPMLBoundaryManager::FillCPMLCoefficients() {

    float pml_reg_len = this->mpParameters->GetBoundaryLength();
    float dh = 0.0;
    float *coeff_a;
    float *coeff_b;
    if (pml_reg_len == 0) {
        pml_reg_len = 1;
    }
    float d0 = 0;
    // Case x :
    if (DIRECTION_ == 1) {
        dh = this->mpGridBox->GetCellDimensions(X_AXIS);
        coeff_a = this->coeff_a_x->GetNativePointer();
        coeff_b = this->coeff_b_x->GetNativePointer();
        // Case z :
    } else if (DIRECTION_ == 2) {
        dh = this->mpGridBox->GetCellDimensions(Z_AXIS);
        coeff_a = this->coeff_a_z->GetNativePointer();
        coeff_b = this->coeff_b_z->GetNativePointer();
        // Case y :
    }
    float dt = this->mpGridBox->GetDT();
    // Compute damping vector
    d0 = (-logf(this->mReflectCoefficient) * ((3 * max_vel) / (pml_reg_len * dh)) *
          this->mRelaxCoefficient) /
         pml_reg_len;

#pragma omp parallel for shared(coeff_a, coeff_b, d0)
    for (int i = this->mpParameters->GetBoundaryLength(); i > 0; i--) {
        float damping_ratio = PWR2(i) * d0;
        coeff_a[i - 1] = expf(-dt * (damping_ratio + this->mShiftRatio));
        coeff_b[i - 1] = (damping_ratio / (damping_ratio + this->mShiftRatio)) *
                         (coeff_a[i - 1] - 1);
    }
}

template<int DIRECTION_, bool OPPOSITE_, int HALF_LENGTH_>
void CPMLBoundaryManager::CalculateFirstAuxiliary() {
    // direction 1 means in x , direction 2 means in z , else means in y;

    float *curr_base = this->mpGridBox->Get(WAVE | GB_PRSS | PREV | DIR_Z)->GetNativePointer();

    int wnx = this->mpGridBox->GetActualWindowSize(X_AXIS);
    int wnz = this->mpGridBox->GetActualWindowSize(Z_AXIS);

    float dx = this->mpGridBox->GetCellDimensions(X_AXIS);
    float dz = this->mpGridBox->GetCellDimensions(Z_AXIS);


    int nx = this->mpGridBox->GetActualGridSize(X_AXIS);
    int nz = this->mpGridBox->GetActualGridSize(Z_AXIS);

    int bound_length = this->mpParameters->GetBoundaryLength();

    int block_x = this->mpParameters->GetBlockX();
    int block_z = this->mpParameters->GetBlockZ();

    int nxEnd = this->mpGridBox->GetLogicalWindowSize(X_AXIS) - HALF_LENGTH_;
    int nzEnd = this->mpGridBox->GetLogicalWindowSize(Z_AXIS) - HALF_LENGTH_;

    int wnxnz = wnx * wnz;

    int distance_unit;
    float dh;
    float *aux, *coeff_a, *coeff_b;
    int z_start = HALF_LENGTH_;
    int x_start = HALF_LENGTH_;

    int WIDTH = bound_length + 2 * HALF_LENGTH_;

    // decides the jump step for the stencil
    if (DIRECTION_ == 1) {
        distance_unit = 1;
        dh = dx;
        coeff_a = coeff_a_x->GetNativePointer();
        coeff_b = coeff_b_x->GetNativePointer();
        if (!OPPOSITE_) {
            x_start = HALF_LENGTH_;
            nxEnd = bound_length + HALF_LENGTH_;
            aux = aux_1_x_up->GetNativePointer();
        } else {
            x_start = this->mpGridBox->GetLogicalWindowSize(X_AXIS) - HALF_LENGTH_ - bound_length;
            nxEnd = this->mpGridBox->GetLogicalWindowSize(X_AXIS) - HALF_LENGTH_;
            aux = aux_1_x_down->GetNativePointer();
        }

    } else if (DIRECTION_ == 2) {
        distance_unit = wnx;
        dh = dz;
        coeff_a = coeff_a_z->GetNativePointer();
        coeff_b = coeff_b_z->GetNativePointer();
        if (!OPPOSITE_) {
            z_start = HALF_LENGTH_;
            nzEnd = bound_length + HALF_LENGTH_;
            aux = this->aux_1_z_up->GetNativePointer();
        } else {
            z_start = this->mpGridBox->GetLogicalWindowSize(Z_AXIS) - HALF_LENGTH_ - bound_length;
            nzEnd = this->mpGridBox->GetLogicalWindowSize(Z_AXIS) - HALF_LENGTH_;
            aux = this->aux_1_z_down->GetNativePointer();
        }

    }
    float first_coeff_h[HALF_LENGTH_ + 1];
    int distance[HALF_LENGTH_ + 1];
    for (int i = 0; i < HALF_LENGTH_ + 1; i++) {
        first_coeff_h[i] = this->mpParameters->GetFirstDerivativeFDCoefficient()[i] / dh;
        distance[i] = distance_unit * i;
    }


#pragma omp parallel default(shared)
    {
#pragma omp for schedule(static, 1) collapse(2)
        for (int bz = z_start; bz < nzEnd; bz += block_z) {
            for (int bx = x_start; bx < nxEnd; bx += block_x) {
                // Calculate the endings appropriately (Handle remainder of the cache
                // blocking loops).
                int ixEnd = fmin(bx + block_x, nxEnd);
                int izEnd = fmin(bz + block_z, nzEnd);
                for (int iz = bz; iz < izEnd; iz++) {
                    int offset = iz * wnx;
                    float *curr = curr_base + offset;
#pragma ivdep
                    for (int ix = bx; ix < ixEnd; ix++) {
                        float value = 0.0;
#if FMA_ARCH
                        // Calculate Finite Difference in the z-direction.
                        value = fma(curr[ix], first_coeff_h[0], value);
                        value = fma(curr[ix - distance[1]], -first_coeff_h[1], value);
                        value = fma(curr[ix + distance[1]], first_coeff_h[1], value);
                        if (half_length > 1) {
                            value = fma(curr[ix - distance[2]], -first_coeff_h[2], value);
                            value = fma(curr[ix + distance[2]], first_coeff_h[2], value);
                        }
                        if (half_length > 2) {
                            value = fma(curr[ix - distance[3]], -first_coeff_h[3], value);
                            value = fma(curr[ix + distance[3]], first_coeff_h[3], value);
                            value = fma(curr[ix - distance[4]], -first_coeff_h[4], value);
                            value = fma(curr[ix + distance[4]], first_coeff_h[4], value);
                        }
                        if (half_length > 4) {
                            value = fma(curr[ix - distance[5]], -first_coeff_h[5], value);
                            value = fma(curr[ix + distance[5]], first_coeff_h[5], value);
                            value = fma(curr[ix - distance[6]], -first_coeff_h[6], value);
                            value = fma(curr[ix + distance[6]], first_coeff_h[6], value);
                        }
                        if (half_length > 6) {
                            value = fma(curr[ix - distance[7]], -first_coeff_h[7], value);
                            value = fma(curr[ix + distance[7]], first_coeff_h[7], value);
                            value = fma(curr[ix - distance[8]], -first_coeff_h[8], value);
                            value = fma(curr[ix + distance[8]], first_coeff_h[8], value);
                        }
#else
                        value = fma(curr[ix], first_coeff_h[0], value);
                        value = fma((-curr[ix - distance[1]] + curr[ix + distance[1]]),
                                    first_coeff_h[1], value);
                        if (HALF_LENGTH_ > 1) {
                            value = fma((-curr[ix - distance[2]] + curr[ix + distance[2]]),
                                        first_coeff_h[2], value);
                        }
                        if (HALF_LENGTH_ > 2) {
                            value = fma((-curr[ix - distance[3]] + curr[ix + distance[3]]),
                                        first_coeff_h[3], value);
                            value = fma((-curr[ix - distance[4]] + curr[ix + distance[4]]),
                                        first_coeff_h[4], value);
                        }
                        if (HALF_LENGTH_ > 4) {
                            value = fma((-curr[ix - distance[5]] + curr[ix + distance[5]]),
                                        first_coeff_h[5], value);
                            value = fma((-curr[ix - distance[6]] + curr[ix + distance[6]]),
                                        first_coeff_h[6], value);
                        }
                        if (HALF_LENGTH_ > 6) {
                            value = fma((-curr[ix - distance[7]] + curr[ix + distance[7]]),
                                        first_coeff_h[7], value);
                            value = fma((-curr[ix - distance[8]] + curr[ix + distance[8]]),
                                        first_coeff_h[8], value);
                        }
#endif
                        int index = 0, coeff_ind = 0;

                        if (DIRECTION_ == 1) { // case x
                            if (OPPOSITE_) {
                                coeff_ind = ix - x_start;
                                index =
                                        iz * WIDTH + (ix - x_start + HALF_LENGTH_);
                            } else {
                                coeff_ind = bound_length - ix + HALF_LENGTH_ - 1;
                                index = iz * WIDTH + ix;
                            }
                        } else if (DIRECTION_ == 2) { // case z
                            if (OPPOSITE_) {
                                coeff_ind = iz - z_start;
                                index = (iz - z_start + HALF_LENGTH_) * wnx + ix;
                            } else {
                                coeff_ind = bound_length - iz + HALF_LENGTH_ - 1;
                                index = iz * wnx + ix;
                            }
                        }
                        aux[index] =
                                coeff_a[coeff_ind] * aux[index] + coeff_b[coeff_ind] * value;
                    }
                }
            }
        }
    }
}

template<int direction, bool OPPOSITE_, int half_length>
void CPMLBoundaryManager::CalculateCPMLValue() {
    // direction 1 means in x , direction 2 means in z , else means in y;

    float *curr_base = this->mpGridBox->Get(WAVE | GB_PRSS | PREV | DIR_Z)->GetNativePointer();
    float *next_base = this->mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();
    int bound_length = this->mpParameters->GetBoundaryLength();


    int wnx = this->mpGridBox->GetActualWindowSize(X_AXIS);
    int wnz = this->mpGridBox->GetActualWindowSize(Z_AXIS);

    float dx = this->mpGridBox->GetCellDimensions(X_AXIS);
    float dz = this->mpGridBox->GetCellDimensions(Z_AXIS);

    int nx = this->mpGridBox->GetActualGridSize(X_AXIS);
    int nz = this->mpGridBox->GetActualGridSize(Z_AXIS);

    int block_x = this->mpParameters->GetBlockX();
    int block_z = this->mpParameters->GetBlockZ();

    float *vel_base = this->mpGridBox->Get(PARM | WIND | GB_VEL)->GetNativePointer();

    int nxEnd = this->mpGridBox->GetLogicalWindowSize(X_AXIS) - half_length;
    int nzEnd = this->mpGridBox->GetLogicalWindowSize(Z_AXIS) - half_length;

    int wnxnz = wnx * wnz;

    int distance_unit;
    float dh;
    float dh2;
    float *aux_first, *aux_second, *coeff_a, *coeff_b;

    int z_start = half_length;
    int x_start = half_length;

    int WIDTH = bound_length + 2 * half_length;

    // decides the jump step for the stencil
    if (direction == 1) {
        distance_unit = 1;
        dh = dx;
        dh2 = dx * dx;
        coeff_a = coeff_a_x->GetNativePointer();
        coeff_b = coeff_b_x->GetNativePointer();
        if (!OPPOSITE_) {
            x_start = half_length;
            nxEnd = bound_length + half_length;
            aux_first = aux_1_x_up->GetNativePointer();
            aux_second = aux_2_x_up->GetNativePointer();
        } else {
            x_start = this->mpGridBox->GetLogicalWindowSize(X_AXIS) - half_length - bound_length;
            nxEnd = this->mpGridBox->GetLogicalWindowSize(X_AXIS) - half_length;
            aux_first = aux_1_x_down->GetNativePointer();
            aux_second = aux_2_x_down->GetNativePointer();
        }
    } else if (direction == 2) {
        distance_unit = wnx;
        dh = dz;
        dh2 = dz * dz;
        coeff_a = coeff_a_z->GetNativePointer();
        coeff_b = coeff_b_z->GetNativePointer();
        if (!OPPOSITE_) {
            z_start = half_length;
            nzEnd = bound_length + half_length;
            aux_first = aux_1_z_up->GetNativePointer();
            aux_second = aux_2_z_up->GetNativePointer();
        } else {
            z_start = this->mpGridBox->GetLogicalWindowSize(Z_AXIS) - half_length - bound_length;
            nzEnd = this->mpGridBox->GetLogicalWindowSize(Z_AXIS) - half_length;
            aux_first = aux_1_z_down->GetNativePointer();
            aux_second = aux_2_z_down->GetNativePointer();
        }
    }

    int distance[half_length + 1];
    float coeff_first_h[half_length + 1];
    float coeff_h[half_length + 1];

    for (int i = 0; i < half_length + 1; i++) {
        distance[i] = i * distance_unit;
        coeff_first_h[i] = this->mpParameters->GetFirstDerivativeFDCoefficient()[i] / dh;
        coeff_h[i] = this->mpParameters->GetSecondDerivativeFDCoefficient()[i] / dh2;
    }

#pragma omp parallel default(shared)
    {
#pragma omp for schedule(static, 1) collapse(2)
        for (int bz = z_start; bz < nzEnd; bz += block_z) {
            for (int bx = x_start; bx < nxEnd; bx += block_x) {
                // Calculate the endings appropriately (Handle remainder of the cache
                // blocking loops).
                int ixEnd = fmin(bx + block_x, nxEnd);
                int izEnd = fmin(bz + block_z, nzEnd);
                // Loop on the elements in the block.
                for (int iz = bz; iz < izEnd; ++iz) {
                    int offset = iz * wnx;
                    float *curr = curr_base + offset;
                    float *vel = vel_base + offset;
                    float *next = next_base + offset;
#pragma ivdep
                    for (int ix = bx; ix < ixEnd; ix++) {
                        float pressure_value = 0.0;
                        float d_first_value = 0.0;
                        int index = 0;
                        int coeff_ind = 0;
                        float sum_val = 0.0;
                        float cpml_val = 0.0;

                        if (direction == 1) { // case x
                            if (OPPOSITE_) {
                                coeff_ind = ix - x_start;
                                index =
                                        iz * WIDTH + (ix - x_start + half_length);
                            } else {
                                coeff_ind = bound_length - ix + half_length - 1;
                                index = iz * WIDTH + ix;
                            }
                        } else if (direction == 2) { // case z
                            if (OPPOSITE_) {
                                coeff_ind = iz - z_start;
                                index = (iz - z_start + half_length) * wnx + ix;
                            } else {
                                coeff_ind = bound_length - iz + half_length - 1;
                                index = iz * wnx + ix;
                            }
                        }
#if FMA_ARCH
                        // Calculate Finite Difference in the z-direction.
                        pressure_value = fma(curr[ix], coeff_h[0], pressure_value);
                        pressure_value =
                                fma(curr[ix - distance[1]], -coeff_h[1], pressure_value);
                        pressure_value =
                                fma(curr[ix + distance[1]], coeff_h[1], pressure_value);
                        if (half_length > 1) {
                            pressure_value =
                                    fma(curr[ix - distance[2]], -coeff_h[2], pressure_value);
                            pressure_value =
                                    fma(curr[ix + distance[2]], coeff_h[2], pressure_value);
                        }
                        if (half_length > 2) {
                            pressure_value =
                                    fma(curr[ix - distance[3]], -coeff_h[3], pressure_value);
                            pressure_value =
                                    fma(curr[ix + distance[3]], coeff_h[3], pressure_value);
                            pressure_value =
                                    fma(curr[ix - distance[4]], -coeff_h[4], pressure_value);
                            pressure_value =
                                    fma(curr[ix + distance[4]], coeff_h[4], pressure_value);
                        }
                        if (half_length > 4) {
                            pressure_value =
                                    fma(curr[ix - distance[5]], -coeff_h[5], pressure_value);
                            pressure_value =
                                    fma(curr[ix + distance[5]], coeff_h[5], pressure_value);
                            pressure_value =
                                    fma(curr[ix - distance[6]], -coeff_h[6], pressure_value);
                            pressure_value =
                                    fma(curr[ix + distance[6]], coeff_h[6], pressure_value);
                        }
                        if (half_length > 6) {
                            pressure_value =
                                    fma(curr[ix - distance[7]], -coeff_h[7], pressure_value);
                            pressure_value =
                                    fma(curr[ix + distance[7]], coeff_h[7], pressure_value);
                            pressure_value =
                                    fma(curr[ix - distance[8]], -coeff_h[8], pressure_value);
                            pressure_value =
                                    fma(curr[ix + distance[8]], coeff_h[8], pressure_value);
                        }
#else
                        pressure_value = fma(curr[ix], coeff_h[0], pressure_value);
                        pressure_value = fma(curr[ix - distance[1]] + curr[ix + distance[1]],
                                             coeff_h[1], pressure_value);
                        if (half_length > 1) {
                            pressure_value = fma(curr[ix - distance[2]] + curr[ix + distance[2]],
                                                 coeff_h[2], pressure_value);
                        }
                        if (half_length > 2) {
                            pressure_value = fma(curr[ix - distance[3]] + curr[ix + distance[3]],
                                                 coeff_h[3], pressure_value);
                            pressure_value = fma(curr[ix - distance[4]] + curr[ix + distance[4]],
                                                 coeff_h[4], pressure_value);
                        }
                        if (half_length > 4) {
                            pressure_value = fma(curr[ix - distance[5]] + curr[ix + distance[5]],
                                                 coeff_h[5], pressure_value);
                            pressure_value = fma(curr[ix - distance[6]] + curr[ix + distance[6]],
                                                 coeff_h[6], pressure_value);
                        }
                        if (half_length > 6) {
                            pressure_value = fma(curr[ix - distance[7]] + curr[ix + distance[7]],
                                                 coeff_h[7], pressure_value);
                            pressure_value = fma(curr[ix - distance[8]] + curr[ix + distance[8]],
                                                 coeff_h[8], pressure_value);
                        }
#endif

                        // calculating the first derivative of the aux1
#if FMA_ARCH
                        // Calculate Finite Difference in the z-direction.
                        d_first_value = fma(aux_first[index], coeff_first_h[0], d_first_value);
                        d_first_value = fma(aux_first[index - distance[1]], -coeff_first_h[1],
                                d_first_value);
                        d_first_value = fma(aux_first[index + distance[1]], coeff_first_h[1],
                                d_first_value);

                        if (half_length > 1) {
                            d_first_value = fma(aux_first[index - distance[2]], -coeff_first_h[2],
                                    d_first_value);
                            d_first_value = fma(aux_first[index + distance[2]], coeff_first_h[2],
                                    d_first_value);
                        }
                        if (half_length > 2) {
                            d_first_value = fma(aux_first[index - distance[3]], -coeff_first_h[3],
                                    d_first_value);
                            d_first_value = fma(aux_first[index + distance[3]], coeff_first_h[3],
                                    d_first_value);
                            d_first_value = fma(aux_first[index - distance[4]], -coeff_first_h[4],
                                    d_first_value);
                            d_first_value = fma(aux_first[index + distance[4]], coeff_first_h[4],
                                    d_first_value);
                        }
                        if (half_length > 4) {
                            d_first_value = fma(aux_first[index - distance[5]], -coeff_first_h[5],
                                    d_first_value);
                            d_first_value = fma(aux_first[index + distance[5]], coeff_first_h[5],
                                    d_first_value);
                            d_first_value = fma(aux_first[index - distance[6]], -coeff_first_h[6],
                                    d_first_value);
                            d_first_value = fma(aux_first[index + distance[6]], coeff_first_h[6],
                                    d_first_value);
                        }
                        if (half_length > 6) {
                            d_first_value = fma(aux_first[index - distance[7]], -coeff_first_h[7],
                                    d_first_value);
                            d_first_value = fma(aux_first[index + distance[7]], coeff_first_h[7],
                                    d_first_value);
                            d_first_value = fma(aux_first[index - distance[8]], -coeff_first_h[8],
                                    d_first_value);
                            d_first_value = fma(aux_first[index + distance[8]], coeff_first_h[8],
                                    d_first_value);
                        }
#else

                        d_first_value = fma(aux_first[index], coeff_first_h[0], d_first_value);
                        d_first_value = fma(-aux_first[index - distance[1]] +
                                            aux_first[index + distance[1]],
                                            coeff_first_h[1], d_first_value);
                        if (half_length > 1) {
                            d_first_value = fma(-aux_first[index - distance[2]] +
                                                aux_first[index + distance[2]],
                                                coeff_first_h[2], d_first_value);
                        }
                        if (half_length > 2) {
                            d_first_value = fma(-aux_first[index - distance[3]] +
                                                aux_first[index + distance[3]],
                                                coeff_first_h[3], d_first_value);
                            d_first_value = fma(-aux_first[index - distance[4]] +
                                                aux_first[index + distance[4]],
                                                coeff_first_h[4], d_first_value);
                        }
                        if (half_length > 4) {
                            d_first_value = fma(-aux_first[index - distance[5]] +
                                                aux_first[index + distance[5]],
                                                coeff_first_h[5], d_first_value);
                            d_first_value = fma(-aux_first[index - distance[6]] +
                                                aux_first[index + distance[6]],
                                                coeff_first_h[6], d_first_value);
                        }
                        if (half_length > 6) {
                            d_first_value = fma(-aux_first[index - distance[7]] +
                                                aux_first[index + distance[7]],
                                                coeff_first_h[7], d_first_value);
                            d_first_value = fma(-aux_first[index - distance[8]] +
                                                aux_first[index + distance[8]],
                                                coeff_first_h[8], d_first_value);
                        }
#endif
                        sum_val = d_first_value + pressure_value;
                        aux_second[index] = coeff_a[coeff_ind] * aux_second[index] +
                                            coeff_b[coeff_ind] * sum_val;
                        cpml_val = vel[ix] * (d_first_value + aux_second[index]);
                        next[ix] += cpml_val;
                    }
                }
            }
        }
    }
}
    