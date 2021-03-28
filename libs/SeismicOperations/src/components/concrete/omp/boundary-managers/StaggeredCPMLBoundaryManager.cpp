//
// Created by amr-nasr on 11/21/19.
//

#include "operations/components/independents/concrete/boundary-managers/extensions/HomogenousExtension.hpp"

#include "operations/components/independents/concrete/boundary-managers/StaggeredCPMLBoundaryManager.hpp"
#include <timer/Timer.h>

#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstring>

#ifndef PWR2
#define PWR2(EXP) ((EXP) * (EXP))
#endif

#define fma(a, b, c) (((a) * (b)) + (c)

using namespace std;
using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;

void StaggeredCPMLBoundaryManager::ApplyBoundary(uint kernel_id) {
    /* Read parameters into local variables to be shared. */
    float *curr_base = this->mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();

    float *vel_base = this->mpGridBox->Get(PARM | WIND | GB_VEL)->GetNativePointer();
    float *den_base = this->mpGridBox->Get(PARM | WIND | GB_DEN)->GetNativePointer();

    float dx = this->mpGridBox->GetCellDimensions(X_AXIS);
    float dz = this->mpGridBox->GetCellDimensions(Z_AXIS);

    int nx = this->mpGridBox->GetActualWindowSize(X_AXIS);
    int nz = this->mpGridBox->GetActualWindowSize(Z_AXIS);

    float *coeff = mpParameters->GetFirstDerivativeStaggeredFDCoefficient();
    HALF_LENGTH half_length = mpParameters->GetHalfLength();
    int b_l = mpParameters->GetBoundaryLength();
    ofstream outfile;

    // Pre-compute the coefficients for each direction.
    float coeff_x[half_length];
    float coeff_z[half_length];
    for (int i = 0; i < half_length; i++) {
        coeff_x[i] = coeff[i + 1];
        coeff_z[i] = coeff[i + 1];
    }

    if (kernel_id == 0) {
        // Apply CPML on pressure.

        float *particle_velocity_current_x =
                this->mpGridBox->Get(WAVE | GB_PRTC | CURR | DIR_X)->GetNativePointer();

        // putting the auxiliary_vel_x right and left
        for (int j = half_length; j < this->mpGridBox->GetLogicalWindowSize(Z_AXIS) - half_length; ++j) {
            for (int i = half_length; i < half_length + b_l; ++i) {
                // offset for the left part
                int offset = i + nx * j;
                // offset for the right part
                int offset_2 = (this->mpGridBox->GetLogicalWindowSize(X_AXIS) - 1) - i + nx * j;

                float value_x_left = 0;
                float value_x_right = 0;
                for (int index = 0; index < half_length; ++index) {
                    value_x_left +=
                            coeff_x[index] *
                            (particle_velocity_current_x[offset + index] -
                             particle_velocity_current_x[offset - (index + 1)]);
                    value_x_right +=
                            coeff_x[index] *
                            (particle_velocity_current_x[offset_2 + index] -
                             particle_velocity_current_x[offset_2 - (index + 1)]);
                }
                // offset for the auxiliary array in x
                int offset_x = (i - half_length) + (j - half_length) * b_l;
                // offset for the coefficient array in x
                int offset3 = b_l + half_length - 1 - i;

                // left boundary
                auxiliary_vel_x_left->GetNativePointer()[offset_x] =
                        small_a_x->GetNativePointer()[offset3] *
                        auxiliary_vel_x_left->GetNativePointer()[offset_x] +
                        (small_b_x->GetNativePointer()[offset3] / (this->mpGridBox->GetCellDimensions(X_AXIS))) *
                        value_x_left;

                // right boundary reversed (index 0 is the outer point
                // (x=nx-1-half_length))
                auxiliary_vel_x_right->GetNativePointer()[offset_x] =
                        small_a_x->GetNativePointer()[offset3] *
                        auxiliary_vel_x_right->GetNativePointer()[offset_x] +
                        (small_b_x->GetNativePointer()[offset3] / (this->mpGridBox->GetCellDimensions(X_AXIS))) *
                        value_x_right;
            }
        }

        float *particle_velocity_current_z =
                this->mpGridBox->Get(WAVE | GB_PRTC | CURR | DIR_Z)->GetNativePointer();

        // putting the auxiliary_vel_z up and down
        for (int j = half_length; j < half_length + b_l; ++j) {
            for (int i = half_length; i < this->mpGridBox->GetLogicalWindowSize(X_AXIS) - half_length; ++i) {
                // offset for the up part
                int offset = i + nx * j;
                // offset for the down part
                int offset_2 = i + (nx * (this->mpGridBox->GetLogicalWindowSize(Z_AXIS) - 1 - j));
                float value_z_up = 0;
                float value_z_down = 0;
                for (int index = 0; index < half_length; ++index) {
                    value_z_up +=
                            coeff_z[index] *
                            (particle_velocity_current_z[offset + index * nx] -
                             particle_velocity_current_z[offset - (index + 1) * nx]);
                    value_z_down +=
                            coeff_z[index] *
                            (particle_velocity_current_z[offset_2 + index * nx] -
                             particle_velocity_current_z[offset_2 - (index + 1) * nx]);
                }
                // offset for the auxiliary array in z
                int offset_z = (i - half_length) +
                               (j - half_length);
                // offset for the coefficient  array in z
                int offset3 = b_l + half_length - j - 1;

                // up boundary
                auxiliary_vel_z_up->GetNativePointer()[offset_z] =
                        small_a_z->GetNativePointer()[offset3] *
                        auxiliary_vel_z_up->GetNativePointer()[offset_z] +
                        (small_b_z->GetNativePointer()[offset3] / (this->mpGridBox->GetCellDimensions(Z_AXIS))) *
                        value_z_up;

                // down boundary (index 0 is the outer point (z=nz-1-half_length))
                auxiliary_vel_z_down->GetNativePointer()[offset_z] =
                        small_a_z->GetNativePointer()[offset3] *
                        auxiliary_vel_z_down->GetNativePointer()[offset_z] +
                        (small_b_z->GetNativePointer()[offset3] / (this->mpGridBox->GetCellDimensions(Z_AXIS))) *
                        value_z_down;
            }
        }

        // change the pressure in the left and right boundaries of x
        for (int j = half_length; j < this->mpGridBox->GetLogicalWindowSize(Z_AXIS) - half_length; ++j) {
            for (int i = half_length; i < half_length + b_l; ++i) {
                // offset for the left part
                int offset = i + nx * j;
                // offset for the right part
                int offset_2 = (this->mpGridBox->GetLogicalWindowSize(X_AXIS) - 1 - i) + nx * j;

                // offset for the outer part for auxiliary whether it is the outer in
                // left or outer in right
                int offset_x = (i - half_length) + (b_l) * (j - half_length);

                // left boundary
                curr_base[offset] =
                        curr_base[offset] -
                        vel_base[offset] * (auxiliary_vel_x_left->GetNativePointer()[offset_x]);

                // right boundary(starting from the outer point (x=nx-1-half_length))
                curr_base[offset_2] =
                        curr_base[offset_2] -
                        vel_base[offset_2] * (auxiliary_vel_x_right->GetNativePointer()[offset_x]);
            }
        }

        // change the pressure in the up and down boundaries of z
        for (int j = half_length; j < half_length + b_l; ++j) {
            for (int i = half_length; i < this->mpGridBox->GetLogicalWindowSize(X_AXIS) - half_length; ++i) {
                // offset for the up boundary
                int offset = i + nx * j;
                // offset of the down boundary
                int offset_2 = i + nx * (this->mpGridBox->GetLogicalWindowSize(Z_AXIS) - 1 - j);
                // size of auxiliary_vel_z_up is x=nx-2*h_l , z=b_l
                // offset for the outer part for auxiliary whether it is the outer in
                // up or outer in down
                int offset_z = (i - half_length) +
                               (this->mpGridBox->GetLogicalWindowSize(X_AXIS) - 2 * half_length) *
                               (j - half_length);

                // up boundary
                curr_base[offset] = curr_base[offset] -
                                    vel_base[offset] * (auxiliary_vel_z_up->GetNativePointer()[offset_z]);

                // down boundary(starting from the outer point (z=nz-1-half_length))
                curr_base[offset_2] =
                        curr_base[offset_2] -
                        vel_base[offset_2] * (auxiliary_vel_z_down->GetNativePointer()[offset_z]);
            }
        }
    } else if (kernel_id == 1) {
        // putting the auxiliary_ptr_x right and left
        for (int j = half_length; j < this->mpGridBox->GetLogicalWindowSize(Z_AXIS) - half_length; ++j) {
            for (int i = half_length; i < half_length + b_l; ++i) {
                // offset for the left part
                int offset = i + nx * j;
                // offset for the right part
                int offset_2 = (this->mpGridBox->GetLogicalWindowSize(X_AXIS) - 1) - i + nx * j;
                float value_x_left = 0;
                float value_x_right = 0;
                for (int index = 0; index < half_length; ++index) {
                    value_x_left += coeff_x[index] * (curr_base[offset + (index + 1)] -
                                                      curr_base[offset - index]);
                    value_x_right +=
                            coeff_x[index] * (curr_base[offset_2 + (index + 1)] -
                                              curr_base[offset_2 - index]);
                }
                // offset for the auxiliary array in x
                int offset_x = (i - half_length) + (j - half_length) * b_l;
                // offset for the coefficient array in x
                int offset3 = b_l + half_length - 1 - i;

                // left boundary
                auxiliary_ptr_x_left->GetNativePointer()[offset_x] =
                        small_a_x->GetNativePointer()[offset3] *
                        auxiliary_ptr_x_left->GetNativePointer()[offset_x] +
                        (small_b_x->GetNativePointer()[offset3] / (this->mpGridBox->GetCellDimensions(X_AXIS))) *
                        value_x_left;

                // right boundary reversed (index 0 is the outer point
                // (x=nx-1-half_length))
                auxiliary_ptr_x_right->GetNativePointer()[offset_x] =
                        small_a_x->GetNativePointer()[offset3] *
                        auxiliary_ptr_x_right->GetNativePointer()[offset_x] +
                        (small_b_x->GetNativePointer()[offset3] / (this->mpGridBox->GetCellDimensions(X_AXIS))) *
                        value_x_right;
            }
        }

        // putting the auxiliary_ptr_z up and down
        for (int j = half_length; j < half_length + b_l; ++j) {
            for (int i = half_length; i < this->mpGridBox->GetLogicalWindowSize(X_AXIS) - half_length; ++i) {
                // offset for the up part
                int offset = i + nx * j;
                // offset for the down part
                int offset_2 = i + (nx * (this->mpGridBox->GetLogicalWindowSize(Z_AXIS) - 1 - j));
                float value_z_up = 0;
                float value_z_down = 0;
                for (int index = 0; index < half_length; ++index) {
                    value_z_up +=
                            coeff_z[index] * (curr_base[offset + (index + 1) * nx] -
                                              curr_base[offset - (index * nx)]);
                    value_z_down +=
                            coeff_z[index] * (curr_base[offset_2 + (index + 1) * nx] -
                                              curr_base[offset_2 - (index * nx)]);
                }
                // offset for the auxiliary array in z
                int offset_z = (i - half_length) +
                               (j - half_length) *
                               (this->mpGridBox->GetLogicalWindowSize(X_AXIS) - 2 * half_length);
                // offset for the coefficient  array in z
                int offset3 = b_l + half_length - j - 1;

                // up boundary
                auxiliary_ptr_z_up->GetNativePointer()[offset_z] =
                        small_a_z->GetNativePointer()[offset3] * auxiliary_ptr_z_up->GetNativePointer()[offset_z] +
                        (small_b_z->GetNativePointer()[offset3] / (this->mpGridBox->GetCellDimensions(Z_AXIS))) *
                        value_z_up;

                // down boundary (index 0 is the outer point (z=nz-1-half_length))
                auxiliary_ptr_z_down->GetNativePointer()[offset_z] =
                        small_a_z->GetNativePointer()[offset3] *
                        auxiliary_ptr_z_down->GetNativePointer()[offset_z] +
                        (small_b_z->GetNativePointer()[offset3] / (this->mpGridBox->GetCellDimensions(Z_AXIS))) *
                        value_z_down;
            }
        }

        float *particle_velocity_current_x =
                this->mpGridBox->Get(WAVE | GB_PRTC | CURR | DIR_X)->GetNativePointer();

        // update the particle_velocity_x in the left and right boundaries
        for (int j = half_length; j < this->mpGridBox->GetLogicalWindowSize(Z_AXIS) - half_length; ++j) {
            for (int i = half_length; i < half_length + b_l; ++i) {
                // offset for the left part
                int offset = i + nx * j;
                // offset for the right part
                int offset_2 = (this->mpGridBox->GetLogicalWindowSize(X_AXIS) - 1 - i) + nx * j;
                // offset for the outer part for auxiliary whether it is the outer in
                // left or outer in right
                int offset_x = (i - half_length) + (b_l) * (j - half_length);

                // left boundary
                particle_velocity_current_x[offset] =
                        particle_velocity_current_x[offset] -
                        den_base[offset] * auxiliary_ptr_x_left->GetNativePointer()[offset_x];

                // right boundary(starting from the outer point (x=nx-1-half_length))
                particle_velocity_current_x[offset_2] =
                        particle_velocity_current_x[offset_2] -
                        den_base[offset_2] * auxiliary_ptr_x_right->GetNativePointer()[offset_x];
            }
        }

        float *particle_velocity_current_z =
                this->mpGridBox->Get(WAVE | GB_PRTC | CURR | DIR_Z)->GetNativePointer();

        // update the particle_velocity_z up and down
        for (int j = half_length; j < half_length + b_l; ++j) {
            for (int i = half_length; i < this->mpGridBox->GetLogicalWindowSize(X_AXIS) - half_length; ++i) {
                // offset for the up boundary
                int offset = i + nx * j;
                // offset of the down boundary
                int offset_2 = i + nx * (this->mpGridBox->GetLogicalWindowSize(Z_AXIS) - 1 - j);
                // size of auxiliary_vel_z_up is x=nx-2*h_l , z=b_l
                // offset for the outer part for auxiliary whether it is the outer in
                // up or outer in down
                int offset_z = (i - half_length) +
                               (this->mpGridBox->GetLogicalWindowSize(X_AXIS) - 2 * half_length) *
                               (j - half_length);

                // up boundary
                particle_velocity_current_z[offset] =
                        particle_velocity_current_z[offset] -
                        den_base[offset] * auxiliary_ptr_z_up->GetNativePointer()[offset_z];

                // down boundary(starting from the outer point (z=nz-1-half_length))
                particle_velocity_current_z[offset_2] =
                        particle_velocity_current_z[offset_2] -
                        den_base[offset_2] * auxiliary_ptr_z_down->GetNativePointer()[offset_z];
            }
        }
    }
}

void StaggeredCPMLBoundaryManager::FillCPMLCoefficients(
        float *coeff_a, float *coeff_b, int boundary_length, float dh, float dt,
        float max_vel, float shift_ratio, float reflect_coeff, float relax_cp) {
    float pml_reg_len = boundary_length;
    if (pml_reg_len == 0) {
        pml_reg_len = 1;
    }
    float d0 = 0;
    // compute damping vector ...
    d0 =
            (-logf(reflect_coeff) * ((3 * max_vel) / (pml_reg_len * dh)) * relax_cp) /
            pml_reg_len;
    for (int i = boundary_length; i > 0; i--) {
        float damping_ratio = i * i * d0;
        coeff_a[i - 1] = expf(-dt * (damping_ratio + shift_ratio));
        coeff_b[i - 1] =
                (damping_ratio / (damping_ratio + shift_ratio)) * (coeff_a[i - 1] - 1);
    }
}

// this function used to reset the auxiliary variables to zero
void StaggeredCPMLBoundaryManager::ZeroAuxiliaryVariables() {
    HALF_LENGTH half_length = mpParameters->GetHalfLength();
    int b_l = mpParameters->GetBoundaryLength();

    // for the auxiliaries in the x boundaries for all y and z
    for (int j = 0; j < this->mpGridBox->GetLogicalWindowSize(Z_AXIS) - 2 * half_length; ++j) {
        for (int i = 0; i < b_l; ++i) {
            int offset = i + b_l * j;
            this->auxiliary_vel_x_left->GetNativePointer()[offset] = 0;
            this->auxiliary_vel_x_right->GetNativePointer()[offset] = 0;
            this->auxiliary_ptr_x_left->GetNativePointer()[offset] = 0;
            this->auxiliary_ptr_x_right->GetNativePointer()[offset] = 0;
        }
    }

    // for the auxiliaries in the z boundaries for all x and y
    for (int j = 0; j < b_l; ++j) {
        for (int i = 0; i < this->mpGridBox->GetLogicalWindowSize(X_AXIS) - 2 * half_length; ++i) {
            int offset =
                    i + (this->mpGridBox->GetLogicalWindowSize(X_AXIS) - 2 * half_length) * j;
            this->auxiliary_vel_z_up->GetNativePointer()[offset] = 0;
            this->auxiliary_vel_z_down->GetNativePointer()[offset] = 0;
            this->auxiliary_ptr_z_up->GetNativePointer()[offset] = 0;
            this->auxiliary_ptr_z_down->GetNativePointer()[offset] = 0;
        }
    }
}
