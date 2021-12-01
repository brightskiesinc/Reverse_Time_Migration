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

#include <operations/components/independents/concrete/boundary-managers/StaggeredCPMLBoundaryManager.hpp>
#include <operations/components/independents/concrete/computation-kernels/BaseComputationHelpers.hpp>

#include "omp.h"

using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;


FORWARD_DECLARE_BOUND_TEMPLATE(StaggeredCPMLBoundaryManager::CalculateVelocityFirstAuxiliary)

FORWARD_DECLARE_BOUND_TEMPLATE(StaggeredCPMLBoundaryManager::CalculateVelocityCPMLValue)

FORWARD_DECLARE_BOUND_TEMPLATE(StaggeredCPMLBoundaryManager::CalculatePressureFirstAuxiliary)

FORWARD_DECLARE_BOUND_TEMPLATE(StaggeredCPMLBoundaryManager::CalculatePressureCPMLValue)

template<bool ADJOINT_, int DIRECTION_, bool OPPOSITE_, int HALF_LENGTH_>
void
StaggeredCPMLBoundaryManager::CalculateVelocityFirstAuxiliary() {

    float dh;
    if (DIRECTION_ == X_AXIS)
        dh = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetCellDimension();
    else if (DIRECTION_ == Z_AXIS)
        dh = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetCellDimension();
    else if (DIRECTION_ == Y_AXIS)
        dh = this->mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetCellDimension();

    float inv_dh = 1.0f / dh;
    int y_start = 0;
    int y_end = 1;
    int z_start = HALF_LENGTH_;
    int z_end = this->mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize() - HALF_LENGTH_;
    int x_start = HALF_LENGTH_;
    int x_end = this->mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize() - HALF_LENGTH_;

    int b_l = mpParameters->GetBoundaryLength();


    int wnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    int wny = this->mpGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
    int wnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

    int lnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize();
    int lny = this->mpGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize();
    int lnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize();


    int wnzwnx = wnx * wnz;
    if (wny > 1) {
        y_start = HALF_LENGTH_;
        y_end = this->mpGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize() - HALF_LENGTH_;

    }
    // Set variables pointers.
    float *aux_variable;
    float *mpSmall_a;
    float *mpSmall_b;
    float *particle_velocity;
    float *coeff = &mpCoeff->GetNativePointer()[1];
    int aux_nx;
    int aux_nz;
    int jump;
    switch (DIRECTION_) {
        case X_AXIS:
            x_end = HALF_LENGTH_ + b_l;
            if (OPPOSITE_) {
                aux_variable = this->mpAuxiliary_vel_x_right->GetNativePointer();
            } else {
                aux_variable = this->mpAuxiliary_vel_x_left->GetNativePointer();
            }
            particle_velocity = this->mpGridBox->Get(WAVE | GB_PRTC | CURR | DIR_X)->GetNativePointer();
            mpSmall_a = this->mpSmall_a_x->GetNativePointer();
            mpSmall_b = this->mpSmall_b_x->GetNativePointer();
            jump = 1;
            aux_nx = b_l;
            aux_nz = lnz - 2 * HALF_LENGTH_;
            break;
        case Y_AXIS:
            y_end = HALF_LENGTH_ + b_l;
            if (OPPOSITE_) {
                aux_variable = this->mpAuxiliary_vel_y_down->GetNativePointer();
            } else {
                aux_variable = this->mpAuxiliary_vel_y_up->GetNativePointer();
            }
            particle_velocity = this->mpGridBox->Get(WAVE | GB_PRTC | CURR | DIR_Y)->GetNativePointer();
            mpSmall_a = this->mpSmall_a_y->GetNativePointer();
            mpSmall_b = this->mpSmall_b_y->GetNativePointer();
            jump = wnzwnx;
            aux_nx = lnx - 2 * HALF_LENGTH_;
            aux_nz = b_l;
            break;
        case Z_AXIS:
            z_end = HALF_LENGTH_ + b_l;
            if (OPPOSITE_) {
                aux_variable = this->mpAuxiliary_vel_z_down->GetNativePointer();
            } else {
                aux_variable = this->mpAuxiliary_vel_z_up->GetNativePointer();
            }
            particle_velocity = this->mpGridBox->Get(WAVE | GB_PRTC | CURR | DIR_Z)->GetNativePointer();
            mpSmall_a = this->mpSmall_a_z->GetNativePointer();
            mpSmall_b = this->mpSmall_b_z->GetNativePointer();
            jump = wnx;
            aux_nx = lnx - 2 * HALF_LENGTH_;
            aux_nz = lnz - 2 * HALF_LENGTH_;
            break;
        default:
            throw bs::base::exceptions::ILLOGICAL_EXCEPTION();
            break;
    }
    int aux_nxnz = aux_nx * aux_nz;
    // Compute.

    int device_num = omp_get_device_num();
#pragma omp target is_device_ptr(mpSmall_a, mpSmall_b, aux_variable, particle_velocity, coeff ) device(device_num)
#pragma omp teams distribute parallel for collapse(3)

    for (int k = y_start; k < y_end; ++k) {
        for (int j = z_start; j < z_end; ++j) {
            for (int i = x_start; i < x_end; ++i) {

                int active_bound_index;
                int real_x = i;
                int real_z = j;
                int real_y = k;
                float value = 0.0f;
                // Setup indices for access.
                if constexpr(DIRECTION_ == X_AXIS) {
                    active_bound_index = i;
                    if constexpr(OPPOSITE_) {
                        real_x = lnx - 1 - i;
                    }
                } else if constexpr(DIRECTION_ == Z_AXIS) {
                    active_bound_index = j;
                    if constexpr(OPPOSITE_) {
                        real_z = lnz - 1 - j;
                    }
                } else {
                    active_bound_index = k;
                    if constexpr(OPPOSITE_) {
                        real_y = lny - 1 - k;
                    }
                }
                // Compute wanted derivative.
                int offset;
                offset = real_x + wnx * real_z + real_y * wnzwnx;
                DERIVE_JUMP_AXIS(offset, jump, 0, 1, -, particle_velocity, coeff, value)
                int offset3 = b_l + HALF_LENGTH_ - 1 - active_bound_index;
                int aux_offset = (i - HALF_LENGTH_) + (j - HALF_LENGTH_) * aux_nx +
                                 (k - y_start) * aux_nxnz;
                aux_variable[aux_offset] =
                        mpSmall_a[offset3] *
                        aux_variable[aux_offset] +
                        mpSmall_b[offset3] * inv_dh *
                        value;
            }
        }
    }
}

template<bool ADJOINT_, int DIRECTION_, bool OPPOSITE_, int HALF_LENGTH_>
void
StaggeredCPMLBoundaryManager::CalculateVelocityCPMLValue() {

    float dh;
    if (DIRECTION_ == X_AXIS)
        dh = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetCellDimension();
    else if (DIRECTION_ == Z_AXIS)
        dh = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetCellDimension();
    else if (DIRECTION_ == Y_AXIS)
        dh = this->mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetCellDimension();

    float inv_dh = 1.0f / dh;
    int y_start = 0;
    int y_end = 1;
    int z_start = HALF_LENGTH_;
    int z_end = this->mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize() - HALF_LENGTH_;
    int x_start = HALF_LENGTH_;
    int x_end = this->mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize() - HALF_LENGTH_;

    int b_l = mpParameters->GetBoundaryLength();

    int wnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    int wny = this->mpGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
    int wnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();
    int lnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize();
    int lny = this->mpGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize();
    int lnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize();


    int wnzwnx = wnx * wnz;
    if (wny > 1) {
        y_start = HALF_LENGTH_;
        y_end = this->mpGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize() - HALF_LENGTH_;

    }
    // Set variables pointers.
    float *aux_variable;
    float *particle_velocity;
    int aux_nx;
    int aux_nz;
    switch (DIRECTION_) {
        case X_AXIS:
            x_end = HALF_LENGTH_ + b_l;
            if (OPPOSITE_) {
                aux_variable = this->mpAuxiliary_ptr_x_right->GetNativePointer();
            } else {
                aux_variable = this->mpAuxiliary_ptr_x_left->GetNativePointer();
            }
            particle_velocity = this->mpGridBox->Get(WAVE | GB_PRTC | CURR | DIR_X)->GetNativePointer();
            aux_nx = b_l;
            aux_nz = lnz - 2 * HALF_LENGTH_;
            break;
        case Y_AXIS:
            y_end = HALF_LENGTH_ + b_l;
            if (OPPOSITE_) {
                aux_variable = this->mpAuxiliary_ptr_y_down->GetNativePointer();
            } else {
                aux_variable = this->mpAuxiliary_ptr_y_up->GetNativePointer();
            }
            particle_velocity = this->mpGridBox->Get(WAVE | GB_PRTC | CURR | DIR_Y)->GetNativePointer();
            aux_nx = lnx - 2 * HALF_LENGTH_;
            aux_nz = b_l;
            break;
        case Z_AXIS:
            z_end = HALF_LENGTH_ + b_l;
            if (OPPOSITE_) {
                aux_variable = this->mpAuxiliary_ptr_z_down->GetNativePointer();
            } else {
                aux_variable = this->mpAuxiliary_ptr_z_up->GetNativePointer();
            }
            particle_velocity = this->mpGridBox->Get(WAVE | GB_PRTC | CURR | DIR_Z)->GetNativePointer();
            aux_nx = lnx - 2 * HALF_LENGTH_;
            aux_nz = lnz - 2 * HALF_LENGTH_;
            break;
        default:
            throw bs::base::exceptions::ILLOGICAL_EXCEPTION();
            break;
    }
    int aux_nxnz = aux_nx * aux_nz;
    float *parameter_base;

    parameter_base = this->mpGridBox->Get(PARM | WIND | GB_DEN)->GetNativePointer();
    
    // Compute.
    int device_num = omp_get_device_num();

#pragma omp target is_device_ptr(parameter_base, aux_variable, particle_velocity ) device(device_num)
#pragma omp teams distribute parallel for collapse(3)

    for (int k = y_start; k < y_end; ++k) {
        for (int j = z_start; j < z_end; ++j) {
            for (int i = x_start; i < x_end; ++i) {
                int real_x = i;
                int real_z = j;
                int real_y = k;
                float value = 0.0f;
                // Setup indices for access.
                if constexpr(DIRECTION_ == X_AXIS) {
                    if constexpr(OPPOSITE_) {
                        real_x = lnx - 1 - i;
                    }
                } else if constexpr(DIRECTION_ == Z_AXIS) {
                    if constexpr(OPPOSITE_) {
                        real_z = lnz - 1 - j;
                    }
                } else {
                    if constexpr(OPPOSITE_) {
                        real_y = lny - 1 - k;
                    }
                }
                // Compute wanted derivative.
                int offset = real_x + wnx * real_z + real_y * wnzwnx;
                int aux_offset = (i - HALF_LENGTH_) + (j - HALF_LENGTH_) * aux_nx +
                                 (k - y_start) * aux_nxnz;
                particle_velocity[offset] =
                        particle_velocity[offset] -
                        parameter_base[offset] * aux_variable[aux_offset];

            }
        }
    }
}

template<bool ADJOINT_, int DIRECTION_, bool OPPOSITE_, int HALF_LENGTH_>
void
StaggeredCPMLBoundaryManager::CalculatePressureFirstAuxiliary() {

    float dh;
    if (DIRECTION_ == X_AXIS)
        dh = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetCellDimension();
    else if (DIRECTION_ == Z_AXIS)
        dh = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetCellDimension();
    else if (DIRECTION_ == Y_AXIS)
        dh = this->mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetCellDimension();

    // float dh_old = this->mpGridBox->GetCellDimensions(DIRECTION_);
    float *curr_base = this->mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();
    float inv_dh = 1.0f / dh;
    int y_start = 0;
    int y_end = 1;
    int z_start = HALF_LENGTH_;
    int z_end = this->mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize() - HALF_LENGTH_;
    int x_start = HALF_LENGTH_;
    int x_end = this->mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize() - HALF_LENGTH_;

    int b_l = mpParameters->GetBoundaryLength();

    int wnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    int wny = this->mpGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
    int wnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();
    int lnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize();
    int lny = this->mpGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize();
    int lnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize();


    int wnzwnx = wnx * wnz;
    if (wny > 1) {
        y_start = HALF_LENGTH_;
        y_end = this->mpGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize() - HALF_LENGTH_;

    }
    // Set variables pointers.
    float *aux_variable;
    float *mpSmall_a;
    float *mpSmall_b;
    float *coeff = &mpCoeff->GetNativePointer()[1];
    int aux_nx;
    int aux_nz;
    int
            jump;
    switch (DIRECTION_) {
        case X_AXIS:
            x_end = HALF_LENGTH_ + b_l;
            if (OPPOSITE_) {
                aux_variable = this->mpAuxiliary_ptr_x_right->GetNativePointer();
            } else {
                aux_variable = this->mpAuxiliary_ptr_x_left->GetNativePointer();
            }
            mpSmall_a = this->mpSmall_a_x->GetNativePointer();
            mpSmall_b = this->mpSmall_b_x->GetNativePointer();
            jump = 1;
            aux_nx = b_l;
            aux_nz = lnz - 2 * HALF_LENGTH_;
            break;
        case Y_AXIS:
            y_end = HALF_LENGTH_ + b_l;
            if (OPPOSITE_) {
                aux_variable = this->mpAuxiliary_ptr_y_down->GetNativePointer();
            } else {
                aux_variable = this->mpAuxiliary_ptr_y_up->GetNativePointer();
            }
            mpSmall_a = this->mpSmall_a_y->GetNativePointer();
            mpSmall_b = this->mpSmall_b_y->GetNativePointer();
            jump = wnzwnx;
            aux_nx = lnx - 2 * HALF_LENGTH_;
            aux_nz = b_l;
            break;
        case Z_AXIS:
            z_end = HALF_LENGTH_ + b_l;
            if (OPPOSITE_) {
                aux_variable = this->mpAuxiliary_ptr_z_down->GetNativePointer();
            } else {
                aux_variable = this->mpAuxiliary_ptr_z_up->GetNativePointer();
            }
            mpSmall_a = this->mpSmall_a_z->GetNativePointer();
            mpSmall_b = this->mpSmall_b_z->GetNativePointer();
            jump = wnx;
            aux_nx = lnx - 2 * HALF_LENGTH_;
            aux_nz = lnz - 2 * HALF_LENGTH_;
            break;
        default:
            throw bs::base::exceptions::ILLOGICAL_EXCEPTION();
            break;
    }
    int aux_nxnz = aux_nx * aux_nz;

    // Compute.
    int device_num = omp_get_device_num();

#pragma omp target is_device_ptr(curr_base, mpSmall_a, mpSmall_b, aux_variable, coeff ) device(device_num)
#pragma omp teams distribute parallel for collapse(3)

    for (int k = y_start; k < y_end; ++k) {
        for (int j = z_start; j < z_end; ++j) {
            for (int i = x_start; i < x_end; ++i) {
                int active_bound_index;
                int real_x = i;
                int real_z = j;
                int real_y = k;
                float value = 0.0f;
                // Setup indices for access.
                if constexpr(DIRECTION_ == X_AXIS) {
                    active_bound_index = i;
                    if constexpr(OPPOSITE_) {
                        real_x = lnx - 1 - i;
                    }
                } else if constexpr(DIRECTION_ == Z_AXIS) {
                    active_bound_index = j;
                    if constexpr(OPPOSITE_) {
                        real_z = lnz - 1 - j;
                    }
                } else {
                    active_bound_index = k;
                    if constexpr(OPPOSITE_) {
                        real_y = lny - 1 - k;
                    }
                }
                // Compute wanted derivative.
                int offset;
                offset = real_x + wnx * real_z + real_y * wnzwnx;
                DERIVE_JUMP_AXIS(offset, jump, 1, 0, -, curr_base, coeff, value)
                int offset3 = b_l + HALF_LENGTH_ - 1 - active_bound_index;
                int aux_offset = (i - HALF_LENGTH_) + (j - HALF_LENGTH_) * aux_nx +
                                 (k - y_start) * aux_nxnz;
                aux_variable[aux_offset] =
                        mpSmall_a[offset3] *
                        aux_variable[aux_offset] +
                        mpSmall_b[offset3] * inv_dh *
                        value;
            }
        }
    }
}

template<bool ADJOINT_, int DIRECTION_, bool OPPOSITE_, int HALF_LENGTH_>
void
StaggeredCPMLBoundaryManager::CalculatePressureCPMLValue() {

    int y_start = 0;
    int y_end = 1;
    int z_start = HALF_LENGTH_;
    int z_end = this->mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize() - HALF_LENGTH_;
    int x_start = HALF_LENGTH_;
    int x_end = this->mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize() - HALF_LENGTH_;

    int b_l = mpParameters->GetBoundaryLength();

    int wnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    int wny = this->mpGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
    int wnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

    int lnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize();
    int lny = this->mpGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize();
    int lnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize();


    int wnzwnx = wnx * wnz;
    if (wny > 1) {
        y_start = HALF_LENGTH_;
        y_end = this->mpGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize() - HALF_LENGTH_;

    }
    // Set variables pointers.
    float *aux_variable;
    int aux_nx;
    int aux_nz;
    switch (DIRECTION_) {
        case X_AXIS:
            x_end = HALF_LENGTH_ + b_l;
            if (OPPOSITE_) {
                aux_variable = this->mpAuxiliary_vel_x_right->GetNativePointer();
            } else {
                aux_variable = this->mpAuxiliary_vel_x_left->GetNativePointer();
            }
            aux_nx = b_l;
            aux_nz = lnz - 2 * HALF_LENGTH_;
            break;
        case Y_AXIS:
            y_end = HALF_LENGTH_ + b_l;
            if (OPPOSITE_) {
                aux_variable = this->mpAuxiliary_vel_y_down->GetNativePointer();
            } else {
                aux_variable = this->mpAuxiliary_vel_y_up->GetNativePointer();
            }
            aux_nx = lnx - 2 * HALF_LENGTH_;
            aux_nz = b_l;
            break;
        case Z_AXIS:
            z_end = HALF_LENGTH_ + b_l;
            if (OPPOSITE_) {
                aux_variable = this->mpAuxiliary_vel_z_down->GetNativePointer();
            } else {
                aux_variable = this->mpAuxiliary_vel_z_up->GetNativePointer();
            }
            aux_nx = lnx - 2 * HALF_LENGTH_;
            aux_nz = lnz - 2 * HALF_LENGTH_;
            break;
        default:
            throw bs::base::exceptions::ILLOGICAL_EXCEPTION();
            break;
    }
    int aux_nxnz = aux_nx * aux_nz;

    float *parameter_base;
    parameter_base = this->mpGridBox->Get(PARM | WIND | GB_VEL)->GetNativePointer();
    
    float *curr_base = this->mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();

    int device_num = omp_get_device_num();

#pragma omp target is_device_ptr(curr_base, parameter_base, aux_variable) device(device_num)
#pragma omp teams distribute parallel for collapse(3)

    for (int k = y_start; k < y_end; ++k) {
        for (int j = z_start; j < z_end; ++j) {
            for (int i = x_start; i < x_end; ++i) {
                int real_x = i;
                int real_z = j;
                int real_y = k;
                // Setup indices for access.
                if constexpr(DIRECTION_ == X_AXIS) {
                    if constexpr(OPPOSITE_) {
                        real_x = lnx - 1 - i;
                    }
                } else if constexpr(DIRECTION_ == Z_AXIS) {
                    if constexpr(OPPOSITE_) {
                        real_z = lnz - 1 - j;
                    }
                } else {
                    if constexpr(OPPOSITE_) {
                        real_y = lny - 1 - k;
                    }
                }
                // Compute wanted derivative.
                int offset = real_x + wnx * real_z + real_y * wnzwnx;
                int aux_offset = (i - HALF_LENGTH_) + (j - HALF_LENGTH_) * aux_nx +
                                 (k - y_start) * aux_nxnz;
                curr_base[offset] = curr_base[offset] - parameter_base[offset] * aux_variable[aux_offset];

            }
        }
    }
}
