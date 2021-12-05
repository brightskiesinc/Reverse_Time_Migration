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

#include <bs/base/backend/Backend.hpp>

#include <operations/components/independents/concrete/boundary-managers/CPMLBoundaryManager.hpp>
#include <operations/components/independents/concrete/computation-kernels/BaseComputationHelpers.hpp>


using namespace cl::sycl;
using namespace bs::base::backend;
using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;

FORWARD_DECLARE_SINGLE_BOUND_TEMPLATE(CPMLBoundaryManager::CalculateFirstAuxiliary)

FORWARD_DECLARE_SINGLE_BOUND_TEMPLATE(CPMLBoundaryManager::CalculateCPMLValue)

template<int DIRECTION_, bool OPPOSITE_, int HALF_LENGTH_>
void CPMLBoundaryManager::CalculateFirstAuxiliary() {
    //DIRECTION_ 1 means in x , DIRECTION_ 2 means in z , else means in y;
    float *curr_base = mpGridBox->Get(WAVE | GB_PRSS | PREV | DIR_Z)->GetNativePointer();


    int wnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    int wny = this->mpGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
    int wnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

    int bound_length = this->mpParameters->GetBoundaryLength();

    int WIDTH = bound_length + 2 * HALF_LENGTH_;

    int ny = mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetLogicalAxisSize();

    int nxEnd = this->mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize() - HALF_LENGTH_;
    int nyEnd = 1;
    int nzEnd = this->mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize() - HALF_LENGTH_;


    int wnxnz = wnx * wnz;

    int y_start = 0;
    if (ny != 1) {
        y_start = HALF_LENGTH_;
        nyEnd = this->mpGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize() - HALF_LENGTH_;
    }

    float *aux = nullptr, *coeff_a = nullptr, *coeff_b = nullptr;
    int z_start = HALF_LENGTH_;
    int x_start = HALF_LENGTH_;
    float *first_coeff_h;
    int *distance;
    float *first_coeff_h_1;
    int *distance_1;

    // decides the jump step for the stencil
    if (DIRECTION_ == X_AXIS) {
        coeff_a = mpCoeffax->GetNativePointer();
        coeff_b = mpCoeffbx->GetNativePointer();
        first_coeff_h = this->mpFirstCoeffx->GetNativePointer();
        distance = this->mpDistanceDim1->GetNativePointer();
        if (!OPPOSITE_) {
            x_start = HALF_LENGTH_;
            nxEnd = bound_length + HALF_LENGTH_;
            aux = this->mpAux1xup->GetNativePointer();
        } else {
            x_start = this->mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize() - HALF_LENGTH_ - bound_length;
            nxEnd = this->mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize() - HALF_LENGTH_;
            aux = this->mpAux1xdown->GetNativePointer();
        }
    } else if (DIRECTION_ == Z_AXIS) {
        coeff_a = this->mpCoeffaz->GetNativePointer();
        coeff_b = this->mpCoeffbz->GetNativePointer();
        first_coeff_h = this->mpFirstCoeffz->GetNativePointer();
        distance = this->mpDistanceDim2->GetNativePointer();
        if (!OPPOSITE_) {
            z_start = HALF_LENGTH_;
            nzEnd = bound_length + HALF_LENGTH_;
            aux = this->mpAux1zup->GetNativePointer();
        } else {
            z_start = this->mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize() - HALF_LENGTH_ - bound_length;
            nzEnd = this->mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize() - HALF_LENGTH_;
            aux = this->mpAux1zdown->GetNativePointer();
        }
    } else {
        coeff_a = this->mpCoeffay->GetNativePointer();
        coeff_b = this->mpCoeffby->GetNativePointer();
        first_coeff_h = this->mpFirstCoeffy->GetNativePointer();
        distance = this->mpDistanceDim3->GetNativePointer();
        if (!OPPOSITE_) {
            y_start = HALF_LENGTH_;
            nyEnd = bound_length + HALF_LENGTH_;
            aux = this->mpAux1yup->GetNativePointer();
        } else {
            y_start = this->mpGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize() - HALF_LENGTH_ - bound_length;
            nyEnd = this->mpGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize() - HALF_LENGTH_;
            aux = this->mpAux1ydown->GetNativePointer();
        }
    }
    first_coeff_h_1 = &first_coeff_h[1];
    distance_1 = &distance[1];

    Backend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
        cgh.parallel_for(range<3>(nxEnd - x_start,
                                  nyEnd - y_start,
                                  nzEnd - z_start),
                         [=](id<3> i) {
                             int ix = i[0] + x_start;
                             int iy = i[1] + y_start;
                             int iz = i[2] + z_start;

                             int offset = iy * wnxnz + iz * wnx;
                             float *curr = curr_base + offset;
                             float value = 0.0;
                             value = fma(curr[ix], first_coeff_h[0], value);
                             DERIVE_ARRAY_AXIS_EQ_OFF(ix, distance_1, -, curr, first_coeff_h_1, value)
                             int index = 0, coeff_ind = 0;
                             if (DIRECTION_ == X_AXIS) { // case x
                                 if (OPPOSITE_) {
                                     coeff_ind = ix - x_start;
                                     index = iy * wnz * WIDTH + iz * WIDTH + (ix - x_start + HALF_LENGTH_);
                                 } else {
                                     coeff_ind = bound_length - ix + HALF_LENGTH_ - 1;
                                     index = iy * wnz * WIDTH + iz * WIDTH + ix;
                                 }
                             } else if (DIRECTION_ == Z_AXIS) { // case z
                                 if (OPPOSITE_) {
                                     coeff_ind = iz - z_start;
                                     index = iy * wnx * WIDTH + (iz - z_start + HALF_LENGTH_) * wnx + ix;
                                 } else {
                                     coeff_ind = bound_length - iz + HALF_LENGTH_ - 1;
                                     index = iy * wnx * WIDTH + iz * wnx + ix;
                                 }
                             } else { // case y
                                 if (OPPOSITE_) {
                                     coeff_ind = iy - y_start;
                                     index = (iy - y_start + HALF_LENGTH_) * wnx * wnz + (iz * wnx) + ix;
                                 } else {
                                     coeff_ind = bound_length - iy + HALF_LENGTH_ - 1;
                                     index = iy * wnx * wnz + (iz * wnx) + ix;
                                 }
                             } // case y
                             aux[index] =
                                     coeff_a[coeff_ind] * aux[index] + coeff_b[coeff_ind] * value;
                         });
    });
    Backend::GetInstance()->GetDeviceQueue()->wait();
}

template<int DIRECTION_, bool OPPOSITE_, int HALF_LENGTH_>
void CPMLBoundaryManager::CalculateCPMLValue() {
    float *curr_base = mpGridBox->Get(WAVE | GB_PRSS | PREV | DIR_Z)->GetNativePointer();
    float *next_base = mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();
    int bound_length = this->mpParameters->GetBoundaryLength();

    int wnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    int wny = this->mpGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
    int wnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

    float *vel_base = mpGridBox->Get(PARM | WIND | GB_VEL)->GetNativePointer();

    int nxEnd = this->mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize() - HALF_LENGTH_;
    int nyEnd = 1;
    int nzEnd = this->mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize() - HALF_LENGTH_;

    int wnxnz = wnx * wnz;

    int y_start = 0;
    if (wny > 1) {
        y_start = HALF_LENGTH_;
        nyEnd = this->mpGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize() - HALF_LENGTH_;
    }

    float *aux_first = nullptr, *aux_second = nullptr,
            *coeff_a = nullptr, *coeff_b = nullptr;

    int z_start = HALF_LENGTH_;
    int x_start = HALF_LENGTH_;

    int WIDTH = bound_length + 2 * HALF_LENGTH_;
    int half_length = HALF_LENGTH_;
    int *distance;
    float *coeff_first_h;
    float *coeff_h;

    // decides the jump step for the stencil
    if (DIRECTION_ == X_AXIS) {
        coeff_a = this->mpCoeffax->GetNativePointer();
        coeff_b = this->mpCoeffbx->GetNativePointer();
        distance = this->mpDistanceDim1->GetNativePointer();
        coeff_first_h = this->mpFirstCoeffx->GetNativePointer();
        coeff_h = this->mpSecondCoeffx->GetNativePointer();
        if (!OPPOSITE_) {
            x_start = HALF_LENGTH_;
            nxEnd = bound_length + half_length;
            aux_first = this->mpAux1xup->GetNativePointer();
            aux_second = this->mpAux2xup->GetNativePointer();
        } else {
            x_start = this->mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize() - half_length - bound_length;
            nxEnd = this->mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize() - half_length;
            aux_first = this->mpAux1xdown->GetNativePointer();
            aux_second = this->mpAux2xdown->GetNativePointer();
        }
    } else if (DIRECTION_ == Z_AXIS) {
        coeff_a = this->mpCoeffaz->GetNativePointer();
        coeff_b = this->mpCoeffbz->GetNativePointer();
        distance = this->mpDistanceDim2->GetNativePointer();
        coeff_first_h = this->mpFirstCoeffz->GetNativePointer();
        coeff_h = this->mpSecondCoeffz->GetNativePointer();
        if (!OPPOSITE_) {
            z_start = half_length;
            nzEnd = bound_length + half_length;
            aux_first = this->mpAux1zup->GetNativePointer();
            aux_second = this->mpAux2zup->GetNativePointer();
        } else {
            z_start = this->mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize() - half_length - bound_length;
            nzEnd = this->mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize() - half_length;
            aux_first = this->mpAux1zdown->GetNativePointer();
            aux_second = this->mpAux2zdown->GetNativePointer();
        }
    } else {
        coeff_a = this->mpCoeffay->GetNativePointer();
        coeff_b = this->mpCoeffby->GetNativePointer();
        distance = this->mpDistanceDim3->GetNativePointer();
        coeff_first_h = this->mpFirstCoeffy->GetNativePointer();
        coeff_h = this->mpSecondCoeffy->GetNativePointer();
        if (!OPPOSITE_) {
            y_start = half_length;
            nyEnd = bound_length + half_length;
            aux_first = this->mpAux1yup->GetNativePointer();
            aux_second = this->mpAux2yup->GetNativePointer();
        } else {
            y_start = this->mpGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize() - half_length - bound_length;
            nyEnd = this->mpGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize() - half_length;
            aux_first = this->mpAux1ydown->GetNativePointer();
            aux_second = this->mpAux2ydown->GetNativePointer();
        }
    }
    auto distance_1 = &distance[1];
    auto coeff_h_1 = &coeff_h[1];
    auto coeff_first_h_1 = &coeff_first_h[1];


    Backend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
        cgh.parallel_for(range<3>(nxEnd - x_start,
                                  nyEnd - y_start,
                                  nzEnd - z_start),
                         [=](id<3> i) {
                             int ix = i[0] + x_start;
                             int iy = i[1] + y_start;
                             int iz = i[2] + z_start;

                             int offset = iy * wnxnz + iz * wnx;
                             float *curr = curr_base + offset;
                             float *vel = vel_base + offset;
                             float *next = next_base + offset;
                             float pressure_value = 0.0;
                             float d_first_value = 0.0;
                             int index = 0;
                             int coeff_ind = 0;
                             float sum_val = 0.0;
                             float cpml_val = 0.0;

                             if (DIRECTION_ == X_AXIS) { // case x
                                 if (OPPOSITE_) {
                                     coeff_ind = ix - x_start;
                                     index =
                                             iy * wnz * WIDTH + iz * WIDTH + (ix - x_start + half_length);
                                 } else {
                                     coeff_ind = bound_length - ix + half_length - 1;
                                     index = iy * wnz * WIDTH + iz * WIDTH + ix;
                                 }
                             } else if (DIRECTION_ == Z_AXIS) { // case z
                                 if (OPPOSITE_) {
                                     coeff_ind = iz - z_start;
                                     index = iy * wnx * WIDTH + (iz - z_start + half_length) * wnx + ix;
                                 } else {
                                     coeff_ind = bound_length - iz + half_length - 1;
                                     index = iy * wnx * WIDTH + iz * wnx + ix;
                                 }
                             } else { // case y
                                 if (OPPOSITE_) {
                                     coeff_ind = iy - y_start;
                                     index = (iy - y_start + half_length) * wnx * wnz + (iz * wnx) + ix;
                                 } else {
                                     coeff_ind = bound_length - iy + half_length - 1;
                                     index = iy * wnx * wnz + (iz * wnx) + ix;
                                 }
                             }// case y
                             pressure_value = fma(curr[ix], coeff_h[0], pressure_value);
                             DERIVE_ARRAY_AXIS_EQ_OFF(ix, distance_1, +, curr, coeff_h_1, pressure_value)

                             //calculating the first dervative of the aux1
                             d_first_value = fma(aux_first[index], coeff_first_h[0], d_first_value);
                             DERIVE_ARRAY_AXIS_EQ_OFF(index, distance_1, -, aux_first, coeff_first_h_1,
                                                      d_first_value)

                             sum_val = d_first_value + pressure_value;
                             aux_second[index] =
                                     coeff_a[coeff_ind] * aux_second[index] + coeff_b[coeff_ind] * sum_val;
                             cpml_val = vel[ix] * (d_first_value + aux_second[index]);
                             next[ix] += cpml_val;

                         });
    });
    Backend::GetInstance()->GetDeviceQueue()->wait();

}
