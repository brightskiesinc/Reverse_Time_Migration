#include "cpml_boundary_manager.h"
#include "concrete-components/boundary_managers/extensions/homogenous_extension.h"
#include <skeleton/components/boundary_manager.h>

#include <algorithm>
#include <cmath>
#include <cstring>

#include <iostream>

#ifndef PWR2
#define PWR2(EXP) ((EXP) * (EXP))
#endif

#define fma(a, b, c) (((a) * (b)) + (c))

CPMLBoundaryManager ::CPMLBoundaryManager(bool use_top_layer,
                                          float reflect_coeff,
                                          float shift_ratio, float relax_cp) {
  this->extension = new HomogenousExtension(use_top_layer);
  this->coeff_a_x = nullptr;
  this->coeff_b_x = nullptr;
  this->coeff_a_z = nullptr;
  this->coeff_b_z = nullptr;
  this->coeff_a_y = nullptr;
  this->coeff_b_y = nullptr;
  this->aux_1_x_up = nullptr;
  this->aux_1_x_down = nullptr;
  this->aux_1_z_up = nullptr;
  this->aux_1_z_down = nullptr;
  this->aux_1_y_up = nullptr;
  this->aux_1_y_down = nullptr;
  this->aux_2_x_up = nullptr;
  this->aux_2_x_down = nullptr;
  this->aux_2_z_up = nullptr;
  this->aux_2_z_down = nullptr;
  this->aux_2_y_up = nullptr;
  this->aux_2_y_down = nullptr;
  this->relax_cp = relax_cp;
  this->shift_ratio = shift_ratio;
  this->reflect_coeff = reflect_coeff;
  this->parameters = nullptr;
  this->grid = nullptr;
  this->max_vel = 0;
}

void CPMLBoundaryManager::InitializeVariables() {
  int nx = grid->grid_size.nx;
  int nz = grid->grid_size.nz;
  int ny = grid->grid_size.ny;

  float dt = grid->dt;
  int bound_length = this->parameters->boundary_length;
  int half_length = this->parameters->half_length;
  float *vel_base = grid->velocity;
  float max_velocity = *max_element(vel_base, vel_base + nx * ny * nz);
  max_velocity = max_velocity / (dt * dt);
  this->max_vel = sqrtf(max_velocity);
  int wnx = grid->window_size.window_nx;
  int wny = grid->window_size.window_ny;
  int wnz = grid->window_size.window_nz;

  this->coeff_a_x = new float[bound_length];
  this->coeff_b_x = new float[bound_length];
  this->coeff_a_z = new float[bound_length];
  this->coeff_b_z = new float[bound_length];

  int width = bound_length + (2 * this->parameters->half_length);
  int y_size = width * wnx * wnz;
  int x_size = width * wny * wnz;
  int z_size = width * wnx * wny;

  this->aux_1_x_up = (float *)mem_allocate(sizeof(float), x_size, "aux");
  this->aux_1_x_down = (float *)mem_allocate(sizeof(float), x_size, "aux");
  this->aux_2_x_up = (float *)mem_allocate(sizeof(float), x_size, "aux");
  this->aux_2_x_down = (float *)mem_allocate(sizeof(float), x_size, "aux");

  memset(this->aux_1_x_up, 0.0, sizeof(float) * x_size);
  memset(this->aux_1_x_down, 0.0, sizeof(float) * x_size);
  memset(this->aux_2_x_up, 0.0, sizeof(float) * x_size);
  memset(this->aux_2_x_down, 0.0, sizeof(float) * x_size);

  this->aux_1_z_up = (float *)mem_allocate(sizeof(float), z_size, "aux");
  this->aux_1_z_down = (float *)mem_allocate(sizeof(float), z_size, "aux");
  this->aux_2_z_up = (float *)mem_allocate(sizeof(float), z_size, "aux");
  this->aux_2_z_down = (float *)mem_allocate(sizeof(float), z_size, "aux");

  memset(this->aux_1_z_up, 0.0, sizeof(float) * z_size);
  memset(this->aux_1_z_down, 0.0, sizeof(float) * z_size);
  memset(this->aux_2_z_up, 0.0, sizeof(float) * z_size);
  memset(this->aux_2_z_down, 0.0, sizeof(float) * z_size);

  FillCpmlCoeff<1>();
  FillCpmlCoeff<2>();
  if (ny > 1) {
    this->coeff_a_y = new float[bound_length];
    this->coeff_b_y = new float[bound_length];

    this->aux_1_y_up = (float *)mem_allocate(sizeof(float), y_size, "aux");
    this->aux_1_y_down = (float *)mem_allocate(sizeof(float), y_size, "aux");
    this->aux_2_y_up = (float *)mem_allocate(sizeof(float), y_size, "aux");
    this->aux_2_y_down = (float *)mem_allocate(sizeof(float), y_size, "aux");

    memset(this->aux_1_y_up, 0.0, sizeof(float) * y_size);
    memset(this->aux_1_y_down, 0.0, sizeof(float) * y_size);
    memset(this->aux_2_y_up, 0.0, sizeof(float) * y_size);
    memset(this->aux_2_y_down, 0.0, sizeof(float) * y_size);

    FillCpmlCoeff<3>();
  }
}

void CPMLBoundaryManager::ResetVariables() {
  int ny = grid->grid_size.ny;

  int wnx = grid->window_size.window_nx;
  int wny = grid->window_size.window_ny;
  int wnz = grid->window_size.window_nz;

  int width =
      this->parameters->boundary_length + (2 * this->parameters->half_length);

  int y_size = width * wnx * wnz;
  int x_size = width * wny * wnz;
  int z_size = width * wnx * wny;

  memset(this->aux_1_x_up, 0.0, sizeof(float) * x_size);
  memset(this->aux_1_x_down, 0.0, sizeof(float) * x_size);
  memset(this->aux_2_x_up, 0.0, sizeof(float) * x_size);
  memset(this->aux_2_x_down, 0.0, sizeof(float) * x_size);

  memset(this->aux_1_z_up, 0.0, sizeof(float) * z_size);
  memset(this->aux_1_z_down, 0.0, sizeof(float) * z_size);
  memset(this->aux_2_z_up, 0.0, sizeof(float) * z_size);
  memset(this->aux_2_z_down, 0.0, sizeof(float) * z_size);
  if (ny > 1) {
    memset(this->aux_1_y_up, 0.0, sizeof(float) * y_size);
    memset(this->aux_1_y_down, 0.0, sizeof(float) * y_size);
    memset(this->aux_2_y_up, 0.0, sizeof(float) * y_size);
    memset(this->aux_2_y_down, 0.0, sizeof(float) * y_size);
  }
}

void CPMLBoundaryManager::ExtendModel() {
  this->extension->ExtendProperty();
  this->InitializeVariables();
}

void CPMLBoundaryManager::ReExtendModel() {
  this->extension->ReExtendProperty();
  this->ResetVariables();
}

CPMLBoundaryManager::~CPMLBoundaryManager() {
  delete this->extension;
  delete[] this->coeff_a_x;
  delete[] this->coeff_b_x;
  delete[] this->coeff_a_z;
  delete[] this->coeff_b_z;
  mem_free(this->aux_1_x_up);
  mem_free(this->aux_1_x_down);
  mem_free(this->aux_1_z_up);
  mem_free(this->aux_1_z_down);
  mem_free(this->aux_2_x_up);
  mem_free(this->aux_2_x_down);
  mem_free(this->aux_2_z_up);
  mem_free(this->aux_2_z_down);
  if (this->aux_1_y_up != nullptr) {
    delete[] this->coeff_a_y;
    delete[] this->coeff_b_y;
    mem_free(this->aux_1_y_up);
    mem_free(this->aux_1_y_down);
    mem_free(this->aux_2_y_up);
    mem_free(this->aux_2_y_down);
  }
}

// make them private to thhe class , extend to th 3d
template <int direction> void CPMLBoundaryManager::FillCpmlCoeff() {

  float pml_reg_len = this->parameters->boundary_length;
  float dh = 0.0;
  float *coeff_a;
  float *coeff_b;
  if (pml_reg_len == 0) {
    pml_reg_len = 1;
  }
  float d0 = 0;
  // Case x :
  if (direction == 1) {
    dh = grid->cell_dimensions.dx;
    coeff_a = this->coeff_a_x;
    coeff_b = this->coeff_b_x;
    // Case z :
  } else if (direction == 2) {
    dh = grid->cell_dimensions.dz;
    coeff_a = this->coeff_a_z;
    coeff_b = this->coeff_b_z;
    // Case y :
  } else {
    dh = grid->cell_dimensions.dy;
    coeff_a = this->coeff_a_y;
    coeff_b = this->coeff_b_y;
  }
  float dt = grid->dt;
  // compute damping vector ...
  d0 = (-logf(this->reflect_coeff) * ((3 * max_vel) / (pml_reg_len * dh)) *
        this->relax_cp) /
       pml_reg_len;
#pragma omp parallel for shared(coeff_a, coeff_b, d0)
  for (int i = this->parameters->boundary_length; i > 0; i--) {
    float damping_ratio = PWR2(i) * d0;
    coeff_a[i - 1] = expf(-dt * (damping_ratio + this->shift_ratio));
    coeff_b[i - 1] = (damping_ratio / (damping_ratio + this->shift_ratio)) *
                     (coeff_a[i - 1] - 1);
  }
}

// the template version
template <int direction, bool opposite, int half_length>
void CPMLBoundaryManager::CalculateFirstAuxilary() {

  int ix, iy, iz;

  // direction 1 means in x , direction 2 means in z , else means in y;

  float *curr_base = grid->pressure_previous;

  int wnx = grid->window_size.window_nx;
  int wny = grid->window_size.window_ny;
  int wnz = grid->window_size.window_nz;

  float dx = grid->cell_dimensions.dx;
  float dy = 1.0;
  float dz = grid->cell_dimensions.dz;

  int nx = grid->grid_size.nx;
  int nz = grid->grid_size.nz;
  int ny = grid->grid_size.ny;
  int bound_length = this->parameters->boundary_length;
  int nxEnd = wnx - half_length;
  int nyEnd = 1;
  int nzEnd = wnz - half_length;

  int wnxnz = wnx * wnz;

  int y_start = 0;
  if (ny != 1) {
    dy = grid->cell_dimensions.dy;
    y_start = half_length;
    nyEnd = wny - half_length;
  }

  int distance_unit;
  float dh;
  float *aux = 0, *coeff_a = 0, *coeff_b = 0;
  int z_start = half_length;
  int x_start = half_length;

  int WIDTH = bound_length + 2 * half_length;

  // decides the jump step for the stencil
  if (direction == 1) {
    distance_unit = 1;
    dh = dx;
    coeff_a = coeff_a_x;
    coeff_b = coeff_b_x;
    if (!opposite) {
      x_start = half_length;
      nxEnd = bound_length + half_length;
      aux = aux_1_x_up;
    } else {
      x_start = wnx - half_length - bound_length;
      nxEnd = wnx - half_length;
      aux = aux_1_x_down;
    }

  } else if (direction == 2) {
    distance_unit = wnx;
    dh = dz;
    coeff_a = coeff_a_z;
    coeff_b = coeff_b_z;
    if (!opposite) {
      z_start = half_length;
      nzEnd = bound_length + half_length;
      aux = this->aux_1_z_up;
    } else {
      z_start = wnz - half_length - bound_length;
      nzEnd = wnz - half_length;
      aux = this->aux_1_z_down;
    }

  } else {
    distance_unit = wnxnz;
    dh = dy;
    coeff_a = coeff_a_y;
    coeff_b = coeff_b_y;
    if (!opposite) {
      y_start = half_length;
      nyEnd = bound_length + half_length;
      aux = aux_1_y_up;
    } else {
      y_start = wny - half_length - bound_length;
      nyEnd = wny - half_length;
      aux = aux_1_y_down;
    }
  }
  float first_coeff_h[half_length + 1];
  int distance[half_length + 1];
  for (int i = 0; i < half_length + 1; i++) {
    first_coeff_h[i] = this->parameters->first_derivative_fd_coeff[i] / dh;
    distance[i] = distance_unit * i;
  }

  for (iy = y_start; iy < nyEnd; iy++) {
    for (iz = z_start; iz < nzEnd; iz++) {
      for (ix = x_start; ix < nxEnd; ix++) {
        int offset = iy * wnxnz + iz * wnx;
        float *curr = curr_base + offset;
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
        if (half_length > 1) {
          value = fma((-curr[ix - distance[2]] + curr[ix + distance[2]]),
                      first_coeff_h[2], value);
        }
        if (half_length > 2) {
          value = fma((-curr[ix - distance[3]] + curr[ix + distance[3]]),
                      first_coeff_h[3], value);
          value = fma((-curr[ix - distance[4]] + curr[ix + distance[4]]),
                      first_coeff_h[4], value);
        }
        if (half_length > 4) {
          value = fma((-curr[ix - distance[5]] + curr[ix + distance[5]]),
                      first_coeff_h[5], value);
          value = fma((-curr[ix - distance[6]] + curr[ix + distance[6]]),
                      first_coeff_h[6], value);
        }
        if (half_length > 6) {
          value = fma((-curr[ix - distance[7]] + curr[ix + distance[7]]),
                      first_coeff_h[7], value);
          value = fma((-curr[ix - distance[8]] + curr[ix + distance[8]]),
                      first_coeff_h[8], value);
        }
#endif
        int index = 0, coeff_ind = 0;

        if (direction == 1) { // case x
          if (opposite) {
            coeff_ind = ix - x_start;
            index =
                iy * wnz * WIDTH + iz * WIDTH + (ix - x_start + half_length);
          } else {
            coeff_ind = bound_length - ix + half_length - 1;
            index = iy * wnz * WIDTH + iz * WIDTH + ix;
          }
        } else if (direction == 2) { // case z
          if (opposite) {
            coeff_ind = iz - z_start;
            index = iy * wnx * WIDTH + (iz - z_start + half_length) * wnx + ix;
          } else {
            coeff_ind = bound_length - iz + half_length - 1;
            index = iy * wnx * WIDTH + iz * wnx + ix;
          }
        } else { // case y
          if (opposite) {
            coeff_ind = iy - y_start;
            index = (iy - y_start + half_length) * wnx * wnz + (iz * wnx) + ix;
          } else {
            coeff_ind = bound_length - iy + half_length - 1;
            index = iy * wnx * wnz + (iz * wnx) + ix;
          }
        } // case y
        aux[index] =
            coeff_a[coeff_ind] * aux[index] + coeff_b[coeff_ind] * value;
      }
    }
  }
}

template <int direction, bool opposite, int half_length>
void CPMLBoundaryManager::CalculateCpmlValue() {
  int ix, iy, iz;

  // direction 1 means in x , direction 2 means in z , else means in y;

  float *curr_base = grid->pressure_previous;
  float *next_base = grid->pressure_current;
  int bound_length = this->parameters->boundary_length;

  int wnx = grid->window_size.window_nx;
  int wny = grid->window_size.window_ny;
  int wnz = grid->window_size.window_nz;

  float dx = grid->cell_dimensions.dx;
  float dy = 1.0;
  float dz = grid->cell_dimensions.dz;

  int nx = grid->grid_size.nx;
  int nz = grid->grid_size.nz;
  int ny = grid->grid_size.ny;

  float *vel_base = grid->window_velocity;

  int nxEnd = wnx - half_length;
  int nyEnd = 1;
  int nzEnd = wnz - half_length;

  int wnxnz = wnx * wnz;
  int nxnz = nx * nz;

  int y_start = 0;
  if (ny > 1) {
    dy = grid->cell_dimensions.dy;
    y_start = half_length;
    nyEnd = wny - half_length;
  }

  int distance_unit;
  float dh;
  float dh2;
  float *aux_first = 0, *aux_second = 0, *coeff_a = 0, *coeff_b = 0;

  int z_start = half_length;
  int x_start = half_length;

  int WIDTH = bound_length + 2 * half_length;

  // decides the jump step for the stencil
  if (direction == 1) {
    distance_unit = 1;
    dh = dx;
    dh2 = dx * dx;
    coeff_a = coeff_a_x;
    coeff_b = coeff_b_x;
    if (!opposite) {
      x_start = half_length;
      nxEnd = bound_length + half_length;
      aux_first = aux_1_x_up;
      aux_second = aux_2_x_up;
    } else {
      x_start = wnx - half_length - bound_length;
      nxEnd = wnx - half_length;
      aux_first = aux_1_x_down;
      aux_second = aux_2_x_down;
    }
  } else if (direction == 2) {
    distance_unit = wnx;
    dh = dz;
    dh2 = dz * dz;
    coeff_a = coeff_a_z;
    coeff_b = coeff_b_z;
    if (!opposite) {
      z_start = half_length;
      nzEnd = bound_length + half_length;
      aux_first = aux_1_z_up;
      aux_second = aux_2_z_up;
    } else {
      z_start = wnz - half_length - bound_length;
      nzEnd = wnz - half_length;
      aux_first = aux_1_z_down;
      aux_second = aux_2_z_down;
    }
  } else {
    distance_unit = wnxnz;
    dh = dy;
    dh2 = dy * dy;
    coeff_a = coeff_a_y;
    coeff_b = coeff_b_y;
    if (!opposite) {
      y_start = half_length;
      nyEnd = bound_length + half_length;
      aux_first = aux_1_y_up;
      aux_second = aux_2_y_up;
    } else {
      y_start = wny - half_length - bound_length;
      nyEnd = wny - half_length;
      aux_first = aux_1_y_down;
      aux_second = aux_2_y_down;
    }
  }

  int distance[half_length + 1];
  float coeff_first_h[half_length + 1];
  float coeff_h[half_length + 1];

  for (int i = 0; i < half_length + 1; i++) {
    distance[i] = i * distance_unit;
    coeff_first_h[i] = this->parameters->first_derivative_fd_coeff[i] / dh;
    coeff_h[i] = this->parameters->second_derivative_fd_coeff[i] / dh2;
  }

  for (iy = y_start; iy < nyEnd; iy++) {
    for (iz = z_start; iz < nzEnd; iz++) {
      for (ix = x_start; ix < nxEnd; ix++) {
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

        if (direction == 1) { // case x
          if (opposite) {
            coeff_ind = ix - x_start;
            index =
                iy * wnz * WIDTH + iz * WIDTH + (ix - x_start + half_length);
          } else {
            coeff_ind = bound_length - ix + half_length - 1;
            index = iy * wnz * WIDTH + iz * WIDTH + ix;
          }
        } else if (direction == 2) { // case z
          if (opposite) {
            coeff_ind = iz - z_start;
            index = iy * wnx * WIDTH + (iz - z_start + half_length) * wnx + ix;
          } else {
            coeff_ind = bound_length - iz + half_length - 1;
            index = iy * wnx * WIDTH + iz * wnx + ix;
          }
        } else { // case y
          if (opposite) {
            coeff_ind = iy - y_start;
            index = (iy - y_start + half_length) * wnx * wnz + (iz * wnx) + ix;
          } else {
            coeff_ind = bound_length - iy + half_length - 1;
            index = iy * wnx * wnz + (iz * wnx) + ix;
          }
        } // case y
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

        // calculating the first dervative of the aux1
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

void CPMLBoundaryManager::SetComputationParameters(
    ComputationParameters *parameters) {
  this->parameters = parameters;
  this->extension->SetHalfLength(parameters->half_length);
  this->extension->SetBoundaryLength(parameters->boundary_length);
}

void CPMLBoundaryManager::SetGridBox(GridBox *grid_box) {
  this->extension->SetGridBox(grid_box);
  this->extension->SetProperty(grid_box->velocity, grid_box->window_velocity);
  this->grid = (AcousticSecondGrid *)(grid_box);
  if (this->grid == nullptr) {
    std::cout << "Not a compatible gridbox : "
                 "expected AcousticSecondGrid"
              << std::endl;
    exit(-1);
  }
}

void CPMLBoundaryManager::ApplyBoundary(uint kernel_id) {
  if (kernel_id == 0) {
    switch (this->parameters->half_length) {
    case O_2:
      ApplyAllCpml<O_2>();
      break;
    case O_4:
      ApplyAllCpml<O_4>();
      break;
    case O_8:
      ApplyAllCpml<O_8>();
      break;
    case O_12:
      ApplyAllCpml<O_12>();
      break;
    case O_16:
      ApplyAllCpml<O_16>();
      break;
    }
  }
}

template <int half_length> void CPMLBoundaryManager::ApplyAllCpml() {
  int ny = grid->grid_size.ny;

  CalculateFirstAuxilary<1, true, half_length>();
  CalculateFirstAuxilary<2, true, half_length>();
  CalculateFirstAuxilary<1, false, half_length>();
  CalculateFirstAuxilary<2, false, half_length>();

  CalculateCpmlValue<1, true, half_length>();
  CalculateCpmlValue<2, true, half_length>();
  CalculateCpmlValue<1, false, half_length>();
  CalculateCpmlValue<2, false, half_length>();
  // 3D --> Add CPML for y-direction.
  if (ny > 1) {
    CalculateFirstAuxilary<3, true, half_length>();
    CalculateFirstAuxilary<3, false, half_length>();

    CalculateCpmlValue<3, true, half_length>();
    CalculateCpmlValue<3, false, half_length>();
  }
}

void CPMLBoundaryManager::AdjustModelForBackward() {
  this->extension->AdjustPropertyForBackward();
  this->ResetVariables();
}
