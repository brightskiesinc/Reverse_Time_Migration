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

#include <operations/components/independents/concrete/boundary-managers/CPMLBoundaryManager.hpp>
#include <operations/components/independents/concrete/computation-kernels/BaseComputationHelpers.hpp>

using namespace std;
using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;

FORWARD_DECLARE_SINGLE_BOUND_TEMPLATE(CPMLBoundaryManager::CalculateFirstAuxiliary)

FORWARD_DECLARE_SINGLE_BOUND_TEMPLATE(CPMLBoundaryManager::CalculateCPMLValue)

template <int DIRECTION_, bool OPPOSITE_, int HALF_LENGTH_>
void CPMLBoundaryManager::CalculateFirstAuxiliary()
{
    int ix, iz;
    float *curr_base = this->mpGridBox->Get(WAVE | GB_PRSS | PREV | DIR_Z)->GetNativePointer();

    int wnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    int wnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

    int nx = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetActualAxisSize();
    int nz = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetActualAxisSize();

    int bound_length = this->mpParameters->GetBoundaryLength();

    int block_x = this->mpParameters->GetBlockX();
    int block_z = this->mpParameters->GetBlockZ();

    int nxEnd = this->mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize() - HALF_LENGTH_;
    int nzEnd = this->mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize() - HALF_LENGTH_;

    float *aux, *coeff_a, *coeff_b;
    int z_start = HALF_LENGTH_;
    int x_start = HALF_LENGTH_;
    float *first_coeff_h;
    int *distance;
    float *first_coeff_h_1;
    int *distance_1;
    int WIDTH = bound_length + 2 * HALF_LENGTH_;

    // decides the jump step for the stencil
    if (DIRECTION_ == X_AXIS)
    {
        coeff_a = mpCoeffax->GetNativePointer();
        coeff_b = mpCoeffbx->GetNativePointer();
        first_coeff_h = this->mpFirstCoeffx->GetNativePointer();
        distance = this->mpDistanceDim1->GetNativePointer();
        if (!OPPOSITE_)
        {
            x_start = HALF_LENGTH_;
            nxEnd = bound_length + HALF_LENGTH_;
            aux = this->mpAux1xup->GetNativePointer();
        }
        else
        {
            x_start = this->mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize() - HALF_LENGTH_ - bound_length;
            nxEnd = this->mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize() - HALF_LENGTH_;
            aux = this->mpAux1xdown->GetNativePointer();
        }
    }
    else if (DIRECTION_ == Z_AXIS)
    {
        coeff_a = this->mpCoeffaz->GetNativePointer();
        coeff_b = this->mpCoeffbz->GetNativePointer();
        first_coeff_h = this->mpFirstCoeffz->GetNativePointer();
        distance = this->mpDistanceDim2->GetNativePointer();
        if (!OPPOSITE_)
        {
            z_start = HALF_LENGTH_;
            nzEnd = bound_length + HALF_LENGTH_;
            aux = this->mpAux1zup->GetNativePointer();
        }
        else
        {
            z_start = this->mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize() - HALF_LENGTH_ - bound_length;
            nzEnd = this->mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize() - HALF_LENGTH_;
            aux = this->mpAux1zdown->GetNativePointer();
        }
    }
    else
    {
        throw bs::base::exceptions::UNSUPPORTED_FEATURE_EXCEPTION();
    }
    
    first_coeff_h_1 = &first_coeff_h[1];
    distance_1 = &distance[1];

#pragma omp parallel default(shared)
    {
#pragma omp for schedule(static, 1) collapse(1)
        for (int bz = z_start; bz < nzEnd; bz += block_z)
        {
            for (int bx = x_start; bx < nxEnd; bx += block_x)
            {
                // Calculate the endings appropriately (Handle remainder of the cache
                // blocking loops).
                int ixEnd = min(bx + block_x, nxEnd);
                int izEnd = min(bz + block_z, nzEnd);

                for (iz = bz; iz < izEnd; iz++)
                {
                    int offset = iz * wnx;
                    float *curr = curr_base + offset;
#pragma ivdep
                    for (ix = bx; ix < ixEnd; ix++)
                    {
                        float value = 0.0;
                        value = fma(curr[ix], first_coeff_h[0], value);
                        DERIVE_ARRAY_AXIS_EQ_OFF(ix, distance_1, -, curr, first_coeff_h_1, value)
                        int index = 0, coeff_ind = 0;
                        if (DIRECTION_ == X_AXIS)
                        { // case x
                            if (OPPOSITE_)
                            {
                                coeff_ind = ix - x_start;
                                index = iz * WIDTH + (ix - x_start + HALF_LENGTH_);
                            }
                            else
                            {
                                coeff_ind = bound_length - ix + HALF_LENGTH_ - 1;
                                index = iz * WIDTH + ix;
                            }
                        }
                        else if (DIRECTION_ == Z_AXIS)
                        { // case z
                            if (OPPOSITE_)
                            {
                                coeff_ind = iz - z_start;
                                index = (iz - z_start + HALF_LENGTH_) * wnx + ix;
                            }
                            else
                            {
                                coeff_ind = bound_length - iz + HALF_LENGTH_ - 1;
                                index = iz * wnx + ix;
                            }
                        }
                    }
                }
            }
        }
    }
}

template <int direction, bool OPPOSITE_, int HALF_LENGTH_>
void CPMLBoundaryManager::CalculateCPMLValue()
{
    int ix, iz;
    // direction 1 means in x , direction 2 means in z , else means in y;

    float *curr_base = this->mpGridBox->Get(WAVE | GB_PRSS | PREV | DIR_Z)->GetNativePointer();
    float *next_base = this->mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();
    int bound_length = this->mpParameters->GetBoundaryLength();
    int half_length = HALF_LENGTH_;

    int wnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    int wnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

    int nx = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetActualAxisSize();
    int nz = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetActualAxisSize();

    int block_x = this->mpParameters->GetBlockX();
    int block_z = this->mpParameters->GetBlockZ();

    float *vel_base = this->mpGridBox->Get(PARM | WIND | GB_VEL)->GetNativePointer();

    int nxEnd = this->mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize() - half_length;
    int nzEnd = this->mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize() - half_length;

    float *aux_first, *aux_second, *coeff_a, *coeff_b;

    int z_start = half_length;
    int x_start = half_length;

    int WIDTH = bound_length + 2 * half_length;

    int *distance;
    float *coeff_first_h;
    float *coeff_h;

    // decides the jump step for the stencil
    if (direction == X_AXIS)
    {
        coeff_a = this->mpCoeffax->GetNativePointer();
        coeff_b = this->mpCoeffbx->GetNativePointer();
        distance = this->mpDistanceDim1->GetNativePointer();
        coeff_first_h = this->mpFirstCoeffx->GetNativePointer();
        coeff_h = this->mpSecondCoeffx->GetNativePointer();
        if (!OPPOSITE_)
        {
            x_start = half_length;
            nxEnd = bound_length + half_length;
            aux_first = this->mpAux1xup->GetNativePointer();
            aux_second = this->mpAux2xup->GetNativePointer();
        }
        else
        {
            x_start = this->mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize() - half_length - bound_length;
            nxEnd = this->mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize() - half_length;
            aux_first = this->mpAux1xdown->GetNativePointer();
            aux_second = this->mpAux2xdown->GetNativePointer();
        }
    }
    else if (direction == Z_AXIS)
    {
        coeff_a = this->mpCoeffaz->GetNativePointer();
        coeff_b = this->mpCoeffbz->GetNativePointer();
        distance = this->mpDistanceDim2->GetNativePointer();
        coeff_first_h = this->mpFirstCoeffz->GetNativePointer();
        coeff_h = this->mpSecondCoeffz->GetNativePointer();
        if (!OPPOSITE_)
        {
            z_start = half_length;
            nzEnd = bound_length + half_length;
            aux_first = this->mpAux1zup->GetNativePointer();
            aux_second = this->mpAux2zup->GetNativePointer();
        }
        else
        {
            z_start = this->mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize() - half_length - bound_length;
            nzEnd = this->mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize() - half_length;
            aux_first = this->mpAux1zdown->GetNativePointer();
            aux_second = this->mpAux2zdown->GetNativePointer();
        }
    }
    else
    {
        throw bs::base::exceptions::UNSUPPORTED_FEATURE_EXCEPTION();
    }

    auto distance_1 = &distance[1];
    auto coeff_h_1 = &coeff_h[1];
    auto coeff_first_h_1 = &coeff_first_h[1];

#pragma omp parallel default(shared)
    {
#pragma omp for schedule(static, 1) collapse(1)
        for (int bz = z_start; bz < nzEnd; bz += block_z)
        {
            for (int bx = x_start; bx < nxEnd; bx += block_x)
            {
                // Calculate the endings appropriately (Handle remainder of the cache
                // blocking loops).
                int ixEnd = fmin(bx + block_x, nxEnd);
                int izEnd = fmin(bz + block_z, nzEnd);
                // Loop on the elements in the block.
                for (iz = bz; iz < izEnd; ++iz)
                {
                    int offset = iz * wnx;
                    float *curr = curr_base + offset;
                    float *vel = vel_base + offset;
                    float *next = next_base + offset;
#pragma ivdep
                    for (ix = bx; ix < ixEnd; ix++)
                    {
                        float pressure_value = 0.0;
                        float d_first_value = 0.0;
                        int index = 0;
                        int coeff_ind = 0;
                        float sum_val = 0.0;
                        float cpml_val = 0.0;

                        if (direction == X_AXIS)
                        { // case x
                            if (OPPOSITE_)
                            {
                                coeff_ind = ix - x_start;
                                index = iz * WIDTH + (ix - x_start + half_length);
                            }
                            else
                            {
                                coeff_ind = bound_length - ix + half_length - 1;
                                index = iz * WIDTH + ix;
                            }
                        }
                        else if (direction == Z_AXIS)
                        { // case z
                            if (OPPOSITE_)
                            {
                                coeff_ind = iz - z_start;
                                index = (iz - z_start + half_length) * wnx + ix;
                            }
                            else
                            {
                                coeff_ind = bound_length - iz + half_length - 1;
                                index = iz * wnx + ix;
                            }
                        }

                        pressure_value = fma(curr[ix], coeff_h[0], pressure_value);
                        DERIVE_ARRAY_AXIS_EQ_OFF(ix, distance_1, +, curr, coeff_h_1, pressure_value)

                        // calculating the first derivative of the aux1
                        d_first_value = fma(aux_first[index], coeff_first_h[0], d_first_value);
                        DERIVE_ARRAY_AXIS_EQ_OFF(index, distance_1, -, aux_first, coeff_first_h_1,
                                                 d_first_value)
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
