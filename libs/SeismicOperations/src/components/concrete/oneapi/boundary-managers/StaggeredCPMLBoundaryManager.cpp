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

#include <bs/base/api/cpp/BSBase.hpp>
#include "operations/components/independents/concrete/boundary-managers/StaggeredCPMLBoundaryManager.hpp"
#include <operations/components/independents/concrete/computation-kernels/BaseComputationHelpers.hpp>
#include <bs/base/exceptions/concrete/NotImplementedException.hpp>
#include <bs/base/exceptions/concrete/IllogicalException.hpp>

using namespace cl::sycl;
using namespace operations::components;
using namespace bs::base::backend;
using namespace operations::common;


FORWARD_DECLARE_BOUND_TEMPLATE(StaggeredCPMLBoundaryManager::CalculateVelocityFirstAuxiliary)

FORWARD_DECLARE_BOUND_TEMPLATE(StaggeredCPMLBoundaryManager::CalculateVelocityCPMLValue)

FORWARD_DECLARE_BOUND_TEMPLATE(StaggeredCPMLBoundaryManager::CalculatePressureFirstAuxiliary)

FORWARD_DECLARE_BOUND_TEMPLATE(StaggeredCPMLBoundaryManager::CalculatePressureCPMLValue)

template<bool ADJOINT_, int DIRECTION_, bool OPPOSITE_, int HALF_LENGTH_>
void StaggeredCPMLBoundaryManager::CalculateVelocityFirstAuxiliary() {

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
    float *aux_variable;
    float *small_a;
    float *small_b;
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
            small_a = this->mpSmall_a_x->GetNativePointer();
            small_b = this->mpSmall_b_x->GetNativePointer();
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
            small_a = this->mpSmall_a_y->GetNativePointer();
            small_b = this->mpSmall_b_y->GetNativePointer();
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
            small_a = this->mpSmall_a_z->GetNativePointer();
            small_b = this->mpSmall_b_z->GetNativePointer();
            jump = wnx;
            aux_nx = lnx - 2 * HALF_LENGTH_;
            aux_nz = lnz - 2 * HALF_LENGTH_;
            break;
        default:
            throw bs::base::exceptions::ILLOGICAL_EXCEPTION();
    }
    int aux_nxnz = aux_nx * aux_nz;


    Backend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
        cgh.parallel_for(range<3>(x_end - x_start,
                                  y_end - y_start,
                                  z_end - z_start), [=](id<3> i) {
            int active_bound_index;
            int x = i[0] + x_start;
            int y = i[1] + y_start;
            int z = i[2] + z_start;
            int real_x = i[0] + x_start;
            int real_y = i[1] + y_start;
            int real_z = i[2] + z_start;
            float value = 0.0f;

            if constexpr (DIRECTION_ == X_AXIS) {
                active_bound_index = x;
                if constexpr(OPPOSITE_) {
                    real_x = lnx - 1 - x;
                }
            } else if constexpr(DIRECTION_ == Z_AXIS) {
                active_bound_index = z;
                if constexpr(OPPOSITE_) {
                    real_z = lnz - 1 - z;
                }
            } else {
                active_bound_index = y;
                if constexpr(OPPOSITE_) {
                    real_y = lny - 1 - y;
                }
            }
            int offset;
            offset = real_x + wnx * real_z + real_y * wnzwnx;

            DERIVE_JUMP_AXIS(offset, jump, 0, 1, -, particle_velocity, coeff, value)

            int offset3 = b_l + HALF_LENGTH_ - 1 - active_bound_index;
            int aux_offset = (x - HALF_LENGTH_) + (z - HALF_LENGTH_) * aux_nx +
                             (y - y_start) * aux_nxnz;
            aux_variable[aux_offset] = small_a[offset3] *
                                       aux_variable[aux_offset] +
                                       small_b[offset3] * inv_dh *
                                       value;
        });
    });
    Backend::GetInstance()->GetDeviceQueue()->wait();
}

template<bool ADJOINT_, int DIRECTION_, bool OPPOSITE_, int HALF_LENGTH_>
void StaggeredCPMLBoundaryManager::CalculateVelocityCPMLValue() {

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


    Backend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
        cgh.parallel_for(range<3>(x_end - x_start,
                                  y_end - y_start,
                                  z_end - z_start),
                         [=](id<3> i) {
                             int x = i[0] + x_start;
                             int y = i[1] + y_start;
                             int z = i[2] + z_start;
                             int real_x = i[0] + x_start;
                             int real_y = i[1] + y_start;
                             int real_z = i[2] + z_start;
                             if constexpr (DIRECTION_ == X_AXIS) {
                                 if constexpr(OPPOSITE_) {
                                     real_x = lnx - 1 - x;
                                 }
                             } else if constexpr(DIRECTION_ == Z_AXIS) {
                                 if constexpr(OPPOSITE_) {
                                     real_z = lnz - 1 - z;
                                 }
                             } else {
                                 if constexpr(OPPOSITE_) {
                                     real_y = lny - 1 - y;
                                 }
                             }
                             int offset = real_x + wnx * real_z + real_y * wnzwnx;
                             int aux_offset = (x - HALF_LENGTH_) + (z - HALF_LENGTH_) * aux_nx +
                                              (y - y_start) * aux_nxnz;
                             particle_velocity[offset] =
                                     particle_velocity[offset] -
                                     parameter_base[offset] * aux_variable[aux_offset];
                         });
    });
    Backend::GetInstance()->GetDeviceQueue()->wait();
}

template<bool ADJOINT_, int DIRECTION_, bool OPPOSITE_, int HALF_LENGTH_>
void StaggeredCPMLBoundaryManager::CalculatePressureFirstAuxiliary() {

    float dh;
    if (DIRECTION_ == X_AXIS)
        dh = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetCellDimension();
    else if (DIRECTION_ == Z_AXIS)
        dh = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetCellDimension();
    else if (DIRECTION_ == Y_AXIS)
        dh = this->mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetCellDimension();

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
    float *aux_variable;
    float *small_a;
    float *small_b;

    float *coeff = &mpCoeff->GetNativePointer()[1];

    int aux_nx;
    int aux_nz;
    int jump;
    switch (DIRECTION_) {
        case X_AXIS:
            x_end = HALF_LENGTH_ + b_l;
            if (OPPOSITE_) {
                aux_variable = this->mpAuxiliary_ptr_x_right->GetNativePointer();
            } else {
                aux_variable = this->mpAuxiliary_ptr_x_left->GetNativePointer();
            }
            small_a = this->mpSmall_a_x->GetNativePointer();
            small_b = this->mpSmall_b_x->GetNativePointer();
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
            small_a = this->mpSmall_a_y->GetNativePointer();
            small_b = this->mpSmall_b_y->GetNativePointer();
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
            small_a = this->mpSmall_a_z->GetNativePointer();
            small_b = this->mpSmall_b_z->GetNativePointer();
            jump = wnx;
            aux_nx = lnx - 2 * HALF_LENGTH_;
            aux_nz = lnz - 2 * HALF_LENGTH_;
            break;
        default:
            throw bs::base::exceptions::ILLOGICAL_EXCEPTION();
    }
    int aux_nxnz = aux_nx * aux_nz;


    Backend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
        cgh.parallel_for(range<3>(x_end - x_start,
                                  y_end - y_start,
                                  z_end - z_start), [=](id<3> i) {

            int active_bound_index;
            int x = i[0] + x_start;
            int y = i[1] + y_start;
            int z = i[2] + z_start;
            int real_x = i[0] + x_start;
            int real_y = i[1] + y_start;
            int real_z = i[2] + z_start;

            float value = 0.0f;

            if constexpr (DIRECTION_ == X_AXIS) {
                active_bound_index = x;
                if constexpr(OPPOSITE_) {
                    real_x = lnx - 1 - x;
                }
            } else if constexpr(DIRECTION_ == Z_AXIS) {
                active_bound_index = z;
                if constexpr(OPPOSITE_) {
                    real_z = lnz - 1 - z;
                }
            } else {
                active_bound_index = y;
                if constexpr(OPPOSITE_) {
                    real_y = lny - 1 - y;
                }
            }
            int offset;
            offset = real_x + wnx * real_z + real_y * wnzwnx;

            DERIVE_JUMP_AXIS(offset, jump, 1, 0, -, curr_base, coeff, value)

            int offset3 = b_l + HALF_LENGTH_ - 1 - active_bound_index;
            int aux_offset = (x - HALF_LENGTH_) + (z - HALF_LENGTH_) * aux_nx +
                             (y - y_start) * aux_nxnz;
            aux_variable[aux_offset] =
                    small_a[offset3] *
                    aux_variable[aux_offset] +
                    small_b[offset3] * inv_dh *
                    value;
        });
    });
    Backend::GetInstance()->GetDeviceQueue()->wait();
}

template<bool ADJOINT_, int DIRECTION_, bool OPPOSITE_, int HALF_LENGTH_>
void StaggeredCPMLBoundaryManager::CalculatePressureCPMLValue() {

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
    Backend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
        cgh.parallel_for(range<3>(x_end - x_start,
                                  y_end - y_start,
                                  z_end - z_start),
                         [=](id<3> i) {
                             int x = i[0] + x_start;
                             int y = i[1] + y_start;
                             int z = i[2] + z_start;
                             int real_x = i[0] + x_start;
                             int real_y = i[1] + y_start;
                             int real_z = i[2] + z_start;
                             if constexpr (DIRECTION_ == X_AXIS) {
                                 if constexpr(OPPOSITE_) {
                                     real_x = lnx - 1 - x;
                                 }
                             } else if constexpr(DIRECTION_ == Z_AXIS) {
                                 if constexpr(OPPOSITE_) {
                                     real_z = lnz - 1 - z;
                                 }
                             } else {
                                 if constexpr(OPPOSITE_) {
                                     real_y = lny - 1 - y;
                                 }
                             }
                             int offset = real_x + wnx * real_z + real_y * wnzwnx;
                             int aux_offset = (x - HALF_LENGTH_) + (z - HALF_LENGTH_) * aux_nx +
                                              (y - y_start) * aux_nxnz;
                             curr_base[offset] =
                                     curr_base[offset] - parameter_base[offset] * aux_variable[aux_offset];
                         });
    });
    Backend::GetInstance()->GetDeviceQueue()->wait();

}
