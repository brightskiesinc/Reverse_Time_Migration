#include <algorithm>
#include <cmath>
#include <concrete-components/boundary_managers/extensions/homogenous_extension.h>
#include <concrete-components/boundary_managers/staggered_cpml_boundary_manager.h>
#include <cstring>
#include <skeleton/components/boundary_manager.h>
#include <skeleton/helpers/timer/timer.hpp>

#include <iostream>

#ifndef PWR2
#define PWR2(EXP) ((EXP) * (EXP))
#endif

#define fma(a, b, c)	(((a) * (b)) + (c)

StaggeredCPMLBoundaryManager ::StaggeredCPMLBoundaryManager(bool use_top_layer,
                                                            float reflect_coeff,
                                                            float shift_ratio,
                                                            float relax_cp) {
  this->extensions.push_back(new HomogenousExtension(use_top_layer));
  this->extensions.push_back(new HomogenousExtension(use_top_layer));
  this->relax_cp = relax_cp;
  this->shift_ratio = shift_ratio;
  this->reflect_coeff = reflect_coeff;
  this->parameters = nullptr;
  this->grid = nullptr;
  this->max_vel = 0;
}

void StaggeredCPMLBoundaryManager::ExtendModel() {
  for (auto const &extension : this->extensions) {
    extension->ExtendProperty();
  }
}

void StaggeredCPMLBoundaryManager::ReExtendModel() {
  for (auto const &extension : this->extensions) {
    extension->ReExtendProperty();
  }
  zero_auxiliary_variables();
}

StaggeredCPMLBoundaryManager::~StaggeredCPMLBoundaryManager() {
  for (auto const &extension : this->extensions) {
    delete extension;
  }
  this->extensions.clear();
}

void StaggeredCPMLBoundaryManager::SetComputationParameters(
    ComputationParameters *parameters) {
  this->parameters = parameters;
  for (auto const &extension : this->extensions) {
    extension->SetHalfLength(parameters->half_length);
    extension->SetBoundaryLength(parameters->boundary_length);
  }
}

void StaggeredCPMLBoundaryManager::SetGridBox(GridBox *grid_box) {
  this->extensions[0]->SetGridBox(grid_box);
  this->extensions[0]->SetProperty(grid_box->velocity, grid_box->window_velocity);
  StaggeredGrid *grid = (StaggeredGrid *)grid_box;
  this->extensions[1]->SetGridBox(grid);
  this->extensions[1]->SetProperty(grid->density, grid->window_density);
  this->grid = grid;
  int nx = this->grid->window_size.window_nx;
  int nz = this->grid->window_size.window_nz;
  int ny = this->grid->window_size.window_ny;
  int b_l = parameters->boundary_length;
  HALF_LENGTH h_l = parameters->half_length;

  // get the size of grid
  int grid_total_size = nx * nz * ny;
  // the size of the boundary in x without half_length is x=b_l and z=nz-2*h_l
  int bound_size_x = b_l * (nz - 2 * h_l);
  // the size of the boundary in z without half_length is x=nx-2*h_l and z=b_l
  int bound_size_z = b_l * (nx - 2 * h_l);

  int bound_size_y = 0;
  bool is_2d;
  int y_start;
  int y_end;

  if (ny == 1) {
    is_2d = 1;
    y_start = 0;
    y_end = 1;
  } else {
    y_start = h_l;
    y_end = ny - h_l;
    is_2d = 0;
    // size of boundary in y without half_length x=nx-2*h_l z=nz-2*h_l y= b_l
    bound_size_y = b_l * (nx - 2 * h_l) * (nz - 2 * h_l);

    // if 3d multoply by y size without half_length which is ny-2*h_l
    bound_size_x = bound_size_x * (ny - 2 * h_l);
    bound_size_z = bound_size_z * (ny - 2 * h_l);
  }

  // allocate the small arrays for coefficients
  small_a_x = new float[b_l];
  small_a_y = new float[b_l];
  small_a_z = new float[b_l];
  small_b_x = new float[b_l];
  small_b_y = new float[b_l];
  small_b_z = new float[b_l];

  // allocate the auxiliary variables for the boundary length for velocity in x
  // direction
  auxiliary_vel_x_left = new float[bound_size_x];
  auxiliary_vel_x_right = new float[bound_size_x];

  // allocate the auxiliary variables for the boundary length for velocity in z
  // direction
  auxiliary_vel_z_up = new float[bound_size_z];
  auxiliary_vel_z_down = new float[bound_size_z];

  // allocate the auxiliary variables for the boundary length for velocity in y
  // direction
  auxiliary_vel_y_up = new float[bound_size_y];
  auxiliary_vel_y_down = new float[bound_size_y];

  // allocate the auxiliary variables for the boundary length for pressure in x
  // direction
  auxiliary_ptr_x_left = new float[bound_size_x];
  auxiliary_ptr_x_right = new float[bound_size_x];

  // allocate the auxiliary variables for the boundary length for pressure in z
  // direction
  auxiliary_ptr_z_up = new float[bound_size_z];
  auxiliary_ptr_z_down = new float[bound_size_z];

  // allocate the auxiliary variables for the boundary length for pressure in y
  // direction
  auxiliary_ptr_y_up = new float[bound_size_y];
  auxiliary_ptr_y_down = new float[bound_size_y];

  // get the maximum velocity
  for (int k = 0; k < ny; ++k) {
    for (int j = 0; j < nz; ++j) {
      for (int i = 0; i < nx; ++i) {
        int offset = i + nx * j + k * nx * nz;
        float velocity_real = grid->velocity[offset];
        if (velocity_real > this->max_vel) {
          this->max_vel = velocity_real;
        }
      }
    }
  }

  // put values for the small arrays
  StaggeredCPMLBoundaryManager::fillCpmlCoeff(
      small_a_x, small_b_x, b_l, grid->cell_dimensions.dx, grid->dt,
      this->max_vel, this->shift_ratio, this->reflect_coeff, this->relax_cp);
  StaggeredCPMLBoundaryManager::fillCpmlCoeff(
      small_a_y, small_b_y, b_l, grid->cell_dimensions.dy, grid->dt,
      this->max_vel, this->shift_ratio, this->reflect_coeff, this->relax_cp);
  StaggeredCPMLBoundaryManager::fillCpmlCoeff(
      small_a_z, small_b_z, b_l, grid->cell_dimensions.dz, grid->dt,
      this->max_vel, this->shift_ratio, this->reflect_coeff, this->relax_cp);
}

void StaggeredCPMLBoundaryManager::ApplyBoundary(uint kernel_id) {

  // Read parameters into local variables to be shared.
  float *curr_base = grid->pressure_current;
  float *vel_base = grid->window_velocity;
  float *density_base = grid->window_density;
  float dx = grid->cell_dimensions.dx;
  float dy;
  float dz = grid->cell_dimensions.dz;
  int nx = grid->window_size.window_nx;
  int nz = grid->window_size.window_nz;
  int ny = grid->window_size.window_ny;
  float dt = grid->dt;
  float *coeff = parameters->first_derivative_staggered_fd_coeff;
  HALF_LENGTH half_length = parameters->half_length;
  int nxnz = nx * nz;
  int b_l = parameters->boundary_length;
  ofstream outfile;

  float k_x = 1 / dx;
  float k_z = 1 / dz;
  float k_y = 1;

  if (ny != 1) {
    dy = grid->cell_dimensions.dy;
    k_y = 1 / dy;
  }
  // Pre-compute the coefficients for each direction.
  float coeff_x[half_length];
  float coeff_y[half_length];
  float coeff_z[half_length];
  for (int i = 0; i < half_length; i++) {
    coeff_x[i] = coeff[i + 1];
    coeff_z[i] = coeff[i + 1];
    if (ny != 1) {
      coeff_y[i] = coeff[i + 1];
    }
  }

  if (kernel_id == 0) {

    // Apply cpml on pressure.
    int is_2d;
    int y_start;
    int y_end;
    int index_2d = 0;
    if (ny == 1) {
      is_2d = 1;
      y_start = 0;
      y_end = 1;

    } else {
      y_start = half_length;
      y_end = ny - half_length;
      is_2d = 0;
      index_2d = half_length;
    }

    // putting the auxiliary_vel_x right and left
    for (int k = y_start; k < y_end; ++k) {
      for (int j = half_length; j < nz - half_length; ++j) {
        for (int i = half_length; i < half_length + b_l; ++i) {
          // offset for the left part
          int offset = i + nx * j + k * nx * nz;
          // offset for the right part
          int offset_2 = (nx - 1) - i + nx * j + k * nx * nz;

          float value_x_left = 0;
          float value_x_right = 0;
          for (int index = 0; index < half_length; ++index) {
            value_x_left +=
                coeff_x[index] *
                (this->grid->particle_velocity_x_current[offset + index] -
                 this->grid->particle_velocity_x_current[offset - (index + 1)]);
            value_x_right +=
                coeff_x[index] *
                (this->grid->particle_velocity_x_current[offset_2 + index] -
                 this->grid
                     ->particle_velocity_x_current[offset_2 - (index + 1)]);
          }
          // offset for the auxiliary array in x
          int offset_x = (i - half_length) + (j - half_length) * b_l +
                         (k - index_2d) * b_l * (nz - 2 * half_length);
          // offset for the coefficient array in x
          int offset3 = b_l + half_length - 1 - i;

          // left boundary
          auxiliary_vel_x_left[offset_x] =
              small_a_x[offset3] * auxiliary_vel_x_left[offset_x] +
              (small_b_x[offset3] / (grid->cell_dimensions.dx)) * value_x_left;

          // right boundary reversed (index 0 is the outer point
          // (x=nx-1-half_length))
          auxiliary_vel_x_right[offset_x] =
              small_a_x[offset3] * auxiliary_vel_x_right[offset_x] +
              (small_b_x[offset3] / (grid->cell_dimensions.dx)) * value_x_right;
        }
      }
    }

    // putting the auxiliary_vel_z up and down
    for (int k = y_start; k < y_end; ++k) {
      for (int j = half_length; j < half_length + b_l; ++j) {
        for (int i = half_length; i < nx - half_length; ++i) {
          // offset for the up part
          int offset = i + nx * j + k * nx * nz;
          // offset for the down part
          int offset_2 = i + (nx * (nz - 1 - j)) + k * nx * nz;
          float value_z_up = 0;
          float value_z_down = 0;
          for (int index = 0; index < half_length; ++index) {
            value_z_up +=
                coeff_z[index] *
                (this->grid->particle_velocity_z_current[offset + index * nx] -
                 this->grid
                     ->particle_velocity_z_current[offset - (index + 1) * nx]);
            value_z_down +=
                coeff_z[index] *
                (this->grid
                     ->particle_velocity_z_current[offset_2 + index * nx] -
                 this->grid->particle_velocity_z_current[offset_2 -
                                                         (index + 1) * nx]);
          }
          // offset for the auxiliary array in z
          int offset_z = (i - half_length) +
                         (j - half_length) * (nx - 2 * half_length) +
                         (k - index_2d) * (nx - 2 * half_length) * b_l;
          // offset for the coefficient  array in z
          int offset3 = b_l + half_length - j - 1;

          // up boundary
          auxiliary_vel_z_up[offset_z] =
              small_a_z[offset3] * auxiliary_vel_z_up[offset_z] +
              (small_b_z[offset3] / (grid->cell_dimensions.dz)) * value_z_up;

          // down boundary (index 0 is the outer point (z=nz-1-half_length))
          auxiliary_vel_z_down[offset_z] =
              small_a_z[offset3] * auxiliary_vel_z_down[offset_z] +
              (small_b_z[offset3] / (grid->cell_dimensions.dz)) * value_z_down;
        }
      }
    }

    // putting the auxiliary for vel_y_up and down
    if (is_2d == 0) {
      for (int k = half_length; k < half_length + b_l; ++k) {
        for (int j = half_length; j < nz - half_length; ++j) {
          for (int i = half_length; i < nx - half_length; ++i) {
            // offset for the up array
            int offset = i + nx * j + k * nx * nz;
            // offset for the down array
            int offset_2 = i + nx * j + (ny - 1 - k) * nx * nz;
            float value_y_up = 0;
            float value_y_down = 0;
            for (int index = 0; index < half_length; ++index) {
              value_y_up +=
                  coeff_y[index] *
                  (this->grid
                       ->particle_velocity_y_current[offset + index * nxnz] -
                   this->grid->particle_velocity_y_current[offset -
                                                           (index + 1) * nxnz]);
              value_y_down +=
                  coeff_y[index] *
                  (this->grid
                       ->particle_velocity_y_current[offset_2 + index * nxnz] -
                   this->grid->particle_velocity_y_current[offset_2 -
                                                           (index + 1) * nxnz]);
            }

            // offset for the auxiliary array in y
            int offset_y = (i - half_length) +
                           (j - half_length) * (nx - 2 * half_length) +
                           (k - index_2d) * (nx - 2 * half_length) *
                               (nz - 2 * half_length);
            // offset for the coefficient array in y
            int offset3 = b_l + half_length - k - 1;

            // up boundary
            auxiliary_vel_y_up[offset_y] =
                small_a_y[offset3] * auxiliary_vel_y_up[offset_y] +
                (small_b_y[offset3] / (grid->cell_dimensions.dy)) * value_y_up;

            // down boundary (index 0 is the outer point (y=ny-1-half_length))
            auxiliary_vel_y_down[offset_y] =
                small_a_y[offset3] * auxiliary_vel_y_down[offset_y] +
                (small_b_y[offset3] / (grid->cell_dimensions.dy)) *
                    value_y_down;
          }
        }
      }
    }
    // change the pressure in the left and right boundaries of x
    for (int k = y_start; k < y_end; ++k) {
      for (int j = half_length; j < nz - half_length; ++j) {
        for (int i = half_length; i < half_length + b_l; ++i) {
          // offset for the left part
          int offset = i + nx * j + k * nxnz;
          // offset for the right part
          int offset_2 = (nx - 1 - i) + nx * j + k * nx * nz;

          // offset for the outer part for auxiliary whether it is the outer in
          // left or outer in right
          int offset_x = (i - half_length) + (b_l) * (j - half_length) +
                         (k - index_2d) * b_l * (nz - 2 * half_length);

          // left boundary
          curr_base[offset] =
              curr_base[offset] -
              vel_base[offset] * (auxiliary_vel_x_left[offset_x]);

          // right boundary(starting from the outer point (x=nx-1-half_length))
          curr_base[offset_2] =
              curr_base[offset_2] -
              vel_base[offset_2] * (auxiliary_vel_x_right[offset_x]);
        }
      }
    }

    // change the pressure in the up and down boundaries of z
    for (int k = y_start; k < y_end; ++k) {
      for (int j = half_length; j < half_length + b_l; ++j) {
        for (int i = half_length; i < nx - half_length; ++i) {
          // offset for the up boundary
          int offset = i + nx * j + k * nxnz;
          // offset of the down boundary
          int offset_2 = i + nx * (nz - 1 - j) + k * nx * nz;
          // size of auxiliary_vel_z_up is x=nx-2*h_l , z=b_l
          // offset for the outer part for auxiliary whether it is the outer in
          // up or outer in down
          int offset_z = (i - half_length) +
                         (nx - 2 * half_length) * (j - half_length) +
                         (k - index_2d) * (nx - 2 * half_length) * b_l;

          // up boundary
          curr_base[offset] = curr_base[offset] -
                              vel_base[offset] * (auxiliary_vel_z_up[offset_z]);

          // down boundary(starting from the outer point (z=nz-1-half_length))
          curr_base[offset_2] =
              curr_base[offset_2] -
              vel_base[offset_2] * (auxiliary_vel_z_down[offset_z]);
        }
      }
    }

    // change the pressure in y up and down
    if (is_2d == 0) {
      for (int k = half_length; k < half_length + b_l; ++k) {
        for (int j = half_length; j < nz - half_length; ++j) {
          for (int i = half_length; i < nx - half_length; ++i) {
            // offset for up part
            int offset = i + nx * j + k * nxnz;
            // offset for down part
            int offset_2 = i + nx * j + (ny - 1 - k) * nx * nz;
            // size of auxiliary_vel_y_up is x=nx-2*h_l , z=nz-2*h_l ,y=b_l
            // offset for the outer part for auxiliary whether it is the outer
            // in up or outer in down
            int offset_y = (i - half_length) +
                           (nx - 2 * half_length) * (j - half_length) +
                           (k - index_2d) * (nx - 2 * half_length) *
                               (nz - 2 * half_length);

            // up boundary
            curr_base[offset] =
                curr_base[offset] -
                vel_base[offset] * (auxiliary_vel_y_up[offset_y]);

            // down boundary(starting from the outer point (y=ny-1-half_length))
            curr_base[offset_2] =
                curr_base[offset_2] -
                vel_base[offset_2] * (auxiliary_vel_y_down[offset_y]);
          }
        }
      }
    }

  } else if (kernel_id == 1) {

    // Apply cpml on particle velocities.
    int is_2d;
    int y_start;
    int y_end;
    int index_2d = 0;
    if (ny == 1) {
      is_2d = 1;
      y_start = 0;
      y_end = 1;

    } else {
      y_start = half_length;
      y_end = ny - half_length;
      is_2d = 0;
      index_2d = half_length;
    }
    // putting the auxiliary_ptr_x right and left
    for (int k = y_start; k < y_end; ++k) {
      for (int j = half_length; j < nz - half_length; ++j) {
        for (int i = half_length; i < half_length + b_l; ++i) {
          // offset for the left part
          int offset = i + nx * j + k * nx * nz;
          // offset for the right part
          int offset_2 = (nx - 1) - i + nx * j + k * nx * nz;
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
          int offset_x = (i - half_length) + (j - half_length) * b_l +
                         (k - index_2d) * b_l * (nz - 2 * half_length);
          // offset for the coefficient array in x
          int offset3 = b_l + half_length - 1 - i;

          // left boundary
          auxiliary_ptr_x_left[offset_x] =
              small_a_x[offset3] * auxiliary_ptr_x_left[offset_x] +
              (small_b_x[offset3] / (grid->cell_dimensions.dx)) * value_x_left;

          // right boundary reversed (index 0 is the outer point
          // (x=nx-1-half_length))
          auxiliary_ptr_x_right[offset_x] =
              small_a_x[offset3] * auxiliary_ptr_x_right[offset_x] +
              (small_b_x[offset3] / (grid->cell_dimensions.dx)) * value_x_right;
        }
      }
    }

    // putting the auxiliary_ptr_z up and down
    for (int k = y_start; k < y_end; ++k) {
      for (int j = half_length; j < half_length + b_l; ++j) {
        for (int i = half_length; i < nx - half_length; ++i) {
          // offset for the up part
          int offset = i + nx * j + k * nx * nz;
          // offset for the down part
          int offset_2 = i + (nx * (nz - 1 - j)) + k * nx * nz;
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
                         (j - half_length) * (nx - 2 * half_length) +
                         (k - index_2d) * (nx - 2 * half_length) * b_l;
          // offset for the coefficient  array in z
          int offset3 = b_l + half_length - j - 1;

          // up boundary
          auxiliary_ptr_z_up[offset_z] =
              small_a_z[offset3] * auxiliary_ptr_z_up[offset_z] +
              (small_b_z[offset3] / (grid->cell_dimensions.dz)) * value_z_up;

          // down boundary (index 0 is the outer point (z=nz-1-half_length))
          auxiliary_ptr_z_down[offset_z] =
              small_a_z[offset3] * auxiliary_ptr_z_down[offset_z] +
              (small_b_z[offset3] / (grid->cell_dimensions.dz)) * value_z_down;
        }
      }
    }

    // putting the auxiliary for ptr_y_up and down
    if (is_2d == 0) {
      for (int k = half_length; k < half_length + b_l; ++k) {
        for (int j = half_length; j < nz - half_length; ++j) {
          for (int i = half_length; i < nx - half_length; ++i) {
            // offset for the up array
            int offset = i + nx * j + k * nx * nz;
            // offset for the down array
            int offset_2 = i + nx * j + (ny - 1 - k) * nx * nz;
            float value_y_up = 0;
            float value_y_down = 0;
            for (int index = 0; index < half_length; ++index) {
              value_y_up +=
                  coeff_y[index] * (curr_base[offset + (index + 1) * nxnz] -
                                    curr_base[offset - (index * nxnz)]);
              value_y_down +=
                  coeff_y[index] * (curr_base[offset_2 + (index + 1) * nxnz] -
                                    curr_base[offset_2 - (index * nxnz)]);
            }
            // offset for the auxiliary array in y
            int offset_y = (i - half_length) +
                           (j - half_length) * (nx - 2 * half_length) +
                           (k - index_2d) * (nx - 2 * half_length) *
                               (nz - 2 * half_length);
            // offset for the coefficient array in y
            int offset3 = b_l + half_length - k - 1;

            // up boundary
            auxiliary_ptr_y_up[offset_y] =
                small_a_y[offset3] * auxiliary_ptr_y_up[offset_y] +
                (small_b_y[offset3] / (grid->cell_dimensions.dy)) * value_y_up;

            // down boundary (index 0 is the outer point (y=ny-1-half_length))
            auxiliary_ptr_y_down[offset_y] =
                small_a_y[offset3] * auxiliary_ptr_y_down[offset_y] +
                (small_b_y[offset3] / (grid->cell_dimensions.dy)) *
                    value_y_down;
          }
        }
      }
    }
    // update the particle_velocity_x in the left and right boundaries
    for (int k = y_start; k < y_end; ++k) {
      for (int j = half_length; j < nz - half_length; ++j) {
        for (int i = half_length; i < half_length + b_l; ++i) {
          // offset for the left part
          int offset = i + nx * j + k * nxnz;
          // offset for the right part
          int offset_2 = (nx - 1 - i) + nx * j + k * nx * nz;
          // offset for the outer part for auxiliary whether it is the outer in
          // left or outer in right
          int offset_x = (i - half_length) + (b_l) * (j - half_length) +
                         (k - index_2d) * b_l * (nz - 2 * half_length);

          // left boundary
          this->grid->particle_velocity_x_current[offset] =
              this->grid->particle_velocity_x_current[offset] -
              density_base[offset] * auxiliary_ptr_x_left[offset_x];

          // right boundary(starting from the outer point (x=nx-1-half_length))
          this->grid->particle_velocity_x_current[offset_2] =
              this->grid->particle_velocity_x_current[offset_2] -
              density_base[offset_2] * auxiliary_ptr_x_right[offset_x];
        }
      }
    }
    // update the particle_velocity_z up and down
    for (int k = y_start; k < y_end; ++k) {
      for (int j = half_length; j < half_length + b_l; ++j) {
        for (int i = half_length; i < nx - half_length; ++i) {
          // offset for the up boundary
          int offset = i + nx * j + k * nxnz;
          // offset of the down boundary
          int offset_2 = i + nx * (nz - 1 - j) + k * nx * nz;
          // size of auxiliary_vel_z_up is x=nx-2*h_l , z=b_l
          // offset for the outer part for auxiliary whether it is the outer in
          // up or outer in down
          int offset_z = (i - half_length) +
                         (nx - 2 * half_length) * (j - half_length) +
                         (k - index_2d) * (nx - 2 * half_length) * b_l;

          // up boundary
          this->grid->particle_velocity_z_current[offset] =
              this->grid->particle_velocity_z_current[offset] -
              density_base[offset] * auxiliary_ptr_z_up[offset_z];

          // down boundary(starting from the outer point (z=nz-1-half_length))
          this->grid->particle_velocity_z_current[offset_2] =
              this->grid->particle_velocity_z_current[offset_2] -
              density_base[offset_2] * auxiliary_ptr_z_down[offset_z];
        }
      }
    }

    // update the particle_velocity_y up and down
    if (is_2d == 0) {
      for (int k = half_length; k < half_length + b_l; ++k) {
        for (int j = half_length; j < nz - half_length; ++j) {
          for (int i = half_length; i < nx - half_length; ++i) {
            // offset for up part
            int offset = i + nx * j + k * nxnz;
            // offset for down part
            int offset_2 = i + nx * j + (ny - 1 - k) * nx * nz;
            // size of auxiliary_vel_y_up is x=nx-2*h_l , z=nz-2*h_l ,y=b_l
            // offset for the outer part for auxiliary whether it is the outer
            // in up or outer in down
            int offset_y = (i - half_length) +
                           (nx - 2 * half_length) * (j - half_length) +
                           (k - index_2d) * (nx - 2 * half_length) *
                               (nz - 2 * half_length);

            // up boundary
            this->grid->particle_velocity_y_current[offset] =
                this->grid->particle_velocity_y_current[offset] -
                density_base[offset] * auxiliary_ptr_y_up[offset_y];

            // down boundary(starting from the outer point (y=ny-1-half_length))
            this->grid->particle_velocity_y_current[offset_2] =
                this->grid->particle_velocity_y_current[offset_2] -
                density_base[offset_2] * auxiliary_ptr_y_down[offset_y];
          }
        }
      }
    }
  }
}

void StaggeredCPMLBoundaryManager::AdjustModelForBackward() {
  for (auto const &extension : this->extensions) {
    extension->AdjustPropertyForBackward();
  }
  zero_auxiliary_variables();
}

void StaggeredCPMLBoundaryManager::fillCpmlCoeff(
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
void StaggeredCPMLBoundaryManager::zero_auxiliary_variables() {
  int nx = grid->window_size.window_nx;
  int nz = grid->window_size.window_nz;
  int ny = grid->window_size.window_ny;
  HALF_LENGTH half_length = parameters->half_length;
  int b_l = parameters->boundary_length;
  int index_2d = 0;
  if (ny != 1) {
    index_2d = half_length;
  }

  // for the auxiliaries in the x boundaries for all y and z
  for (int k = 0; k < ny - 2 * index_2d; ++k) {
    for (int j = 0; j < nz - 2 * half_length; ++j) {
      for (int i = 0; i < b_l; ++i) {
        int offset = i + b_l * j + k * b_l * (nz - 2 * half_length);
        this->auxiliary_vel_x_left[offset] = 0;
        this->auxiliary_vel_x_right[offset] = 0;
        this->auxiliary_ptr_x_left[offset] = 0;
        this->auxiliary_ptr_x_right[offset] = 0;
      }
    }
  }

  // for the auxiliaries in the z boundaries for all x and y
  for (int k = 0; k < ny - 2 * index_2d; ++k) {
    for (int j = 0; j < b_l; ++j) {
      for (int i = 0; i < nx - 2 * half_length; ++i) {
        int offset =
            i + (nx - 2 * half_length) * j + k * (nx - 2 * half_length) * b_l;
        this->auxiliary_vel_z_up[offset] = 0;
        this->auxiliary_vel_z_down[offset] = 0;
        this->auxiliary_ptr_z_up[offset] = 0;
        this->auxiliary_ptr_z_down[offset] = 0;
      }
    }
  }

  // for the auxiliaries in the z boundaries for all x and y
  if (ny != 1) {
    for (int k = 0; k < ny - 2 * index_2d; ++k) {
      for (int j = 0; j < nz - 2 * half_length; ++j) {
        for (int i = 0; i < nx - 2 * half_length; ++i) {
          int offset = i + (nx - 2 * half_length) * j +
                       k * (nx - 2 * half_length) * (nz - 2 * half_length);
          this->auxiliary_vel_y_up[offset] = 0;
          this->auxiliary_vel_y_down[offset] = 0;
          this->auxiliary_ptr_y_up[offset] = 0;
          this->auxiliary_ptr_y_down[offset] = 0;
        }
      }
    }
  }
}
