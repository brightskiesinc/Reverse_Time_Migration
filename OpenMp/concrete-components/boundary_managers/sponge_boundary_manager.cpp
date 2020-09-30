//
// Created by mirnamoawad on 11/21/19.
//

#include "sponge_boundary_manager.h"
#include <cmath>
#include <concrete-components/boundary_managers/extensions/homogenous_extension.h>
#include <iostream>

using namespace std;

// Based on
// https://pubs.geoscienceworld.org/geophysics/article-abstract/50/4/705/71992/A-nonreflecting-boundary-condition-for-discrete?redirectedFrom=fulltext

SpongeBoundaryManager ::SpongeBoundaryManager(bool use_top_layer,
                                              bool is_staggered) {
  this->extensions.push_back(new HomogenousExtension(use_top_layer));
  this->is_staggered = is_staggered;
  if (is_staggered) {
    this->extensions.push_back(new HomogenousExtension(use_top_layer));
  }
}

float SpongeBoundaryManager ::calculation(int index) {
  float value;
  uint bound_length = parameters->boundary_length;
  value = expf(-powf((0.1f / bound_length) * (bound_length - index), 2));
  return value;
}

void SpongeBoundaryManager::ApplyBoundaryOnField(float *next) {
  /*! sponge boundary implementation */
  int nx = grid->window_size.window_nx;
  int ny = grid->window_size.window_ny;
  int nz = grid->window_size.window_nz;
  uint bound_length = parameters->boundary_length;
  uint half_length = parameters->half_length;
  int y_start = half_length + bound_length;
  int y_end = ny - half_length - bound_length;
  if (ny == 1) {
    y_start = 0;
    y_end = 1;
  }
#pragma omp parallel default(shared)
  {
#pragma omp for schedule(static, 1) collapse(2)
  for (int iy = y_start; iy < y_end; iy++) {
    for (int iz = half_length + bound_length - 1; iz >= half_length; iz--) {
#pragma ivdep
      for (int ix = half_length + bound_length;ix <= nx - half_length - bound_length; ix++) {
        next[iy * nx * nz + iz * nx + ix] *=
            this->sponge_coeffs[iz - half_length];
        next[iy * nx * nz + (iz + nz - 2 * iz - 1) * nx + ix] *=
            this->sponge_coeffs[iz - half_length];
      }
    }
  }
  }
#pragma omp parallel default(shared)
    {
#pragma omp for schedule(static, 1) collapse(2)
  for (int iy = y_start; iy < y_end; iy++) {
    for (int iz = half_length + bound_length;iz <= nz - half_length - bound_length; iz++) {
#pragma ivdep
      for (int ix = half_length + bound_length - 1; ix >= half_length; ix--) {
        next[iy * nx * nz + iz * nx + ix] *=
            this->sponge_coeffs[ix - half_length];
        next[iy * nx * nz + iz * nx + (ix + nx - 2 * ix - 1)] *=
            this->sponge_coeffs[ix - half_length];
      }
    }
  }
  }

  if (ny > 1) {
#pragma omp parallel default(shared)
      {
#pragma omp for schedule(static, 1) collapse(2)
    for (int iy = half_length + bound_length - 1; iy >= half_length; iy--) {
      for (int iz = half_length + bound_length;iz <= nz - half_length - bound_length; iz++) {
#pragma ivdep
        for (int ix = half_length + bound_length;ix <= nx - half_length - bound_length; ix++) {
          next[iy * nx * nz + iz * nx + ix] *=
              this->sponge_coeffs[iy - half_length];
          next[(iy + ny - 2 * iy - 1) * nx * nz + iz * nx + ix] *=
              this->sponge_coeffs[iy - half_length];
        }
      }
    }
  }
  }
  int start_y = y_start;
  int end_y = y_end;
  int start_x = half_length;
  int end_x = nx - half_length;
  int start_z = half_length;
  int end_z = nz - half_length;
  int nz_nx = nz * nx;

#pragma omp parallel default(shared)
    {
#pragma omp for schedule(static, 1) collapse(2)
  // Zero-Corners in the boundaries nx-nz boundary intersection--boundaries not
  // needed.
  for (int depth = start_y; depth < end_y; depth++) {
    for (int row = 0; row < bound_length; row++) {
#pragma ivdep
      for (int column = 0; column < bound_length; column++) {
        /*!for values from z = half_length TO z = half_length +BOUND_LENGTH */
        /*! and for x = half_length to x = half_length + BOUND_LENGTH */
        /*! Top left boundary in other words */
        next[depth * nz_nx + (start_z + row) * nx + column + start_x] *=
            min(this->sponge_coeffs[column], this->sponge_coeffs[row]);
        /*!for values from z = nz-half_length TO z =
         * nz-half_length-BOUND_LENGTH*/
        /*! and for x = half_length to x = half_length + BOUND_LENGTH */
        /*! Bottom left boundary in other words */
        next[depth * nz_nx + (end_z - 1 - row) * nx + column + start_x] *=
            min(this->sponge_coeffs[column], this->sponge_coeffs[row]);
        /*!for values from z = half_length TO z = half_length +BOUND_LENGTH */
        /*! and for x = nx-half_length to x = nx-half_length - BOUND_LENGTH */
        /*! Top right boundary in other words */
        next[depth * nz_nx + (start_z + row) * nx + (end_x - 1 - column)] *=
            min(this->sponge_coeffs[column], this->sponge_coeffs[row]);
        /*!for values from z = nz-half_length TO z =
         * nz-half_length-BOUND_LENGTH*/
        /*! and for x = nx-half_length to x = nx - half_length - BOUND_LENGTH */
        /*! Bottom right boundary in other words */
        next[depth * nz_nx + (end_z - 1 - row) * nx + (end_x - 1 - column)] *=
            min(this->sponge_coeffs[column], this->sponge_coeffs[row]);
      }
    }
  }
  }
  // If 3-D, zero corners in the y-x and y-z plans.
  if (ny > 1) {
    // Zero-Corners in the boundaries ny-nz boundary intersection--boundaries
    // not needed.
#pragma omp parallel default(shared)
      {
#pragma omp for schedule(static, 1) collapse(2)
    for (int depth = 0; depth < bound_length; depth++) {
      for (int row = 0; row < bound_length; row++) {
#pragma ivdep
        for (int column = start_x; column < end_x; column++) {
          /*!for values from z = half_length TO z = half_length +BOUND_LENGTH */
          /*! and for y = half_length to y = half_length + BOUND_LENGTH */
          next[(depth + start_y) * nz_nx + (start_z + row) * nx + column] *=
              min(this->sponge_coeffs[depth], this->sponge_coeffs[row]);
          /*!for values from z = nz-half_length TO z =
           * nz-half_length-BOUND_LENGTH*/
          /*! and for y = half_length to y = half_length + BOUND_LENGTH */
          next[(depth + start_y) * nz_nx + (end_z - 1 - row) * nx + column] *=
              min(this->sponge_coeffs[depth], this->sponge_coeffs[row]);
          /*!for values from z = half_length TO z = half_length +BOUND_LENGTH */
          /*! and for y = ny-half_length to y = ny-half_length - BOUND_LENGTH */
          next[(end_y - 1 - depth) * nz_nx + (start_z + row) * nx + column] *=
              min(this->sponge_coeffs[depth], this->sponge_coeffs[row]);
          /*!for values from z = nz-half_length TO z =
           * nz-half_length-BOUND_LENGTH */
          /*! and for y = ny-half_length to y = ny - half_length - BOUND_LENGTH
           */
          next[(end_y - 1 - depth) * nz_nx + (end_z - 1 - row) * nx + column] *=
              min(this->sponge_coeffs[depth], this->sponge_coeffs[row]);
        }
      }
    }
    }
#pragma omp parallel default(shared)
      {
#pragma omp for schedule(static, 1) collapse(2)
    // Zero-Corners in the boundaries nx-ny boundary intersection--boundaries
    // not needed.
    for (int depth = 0; depth < bound_length; depth++) {
      for (int row = start_z; row < end_z; row++) {
#pragma ivdep
        for (int column = 0; column < bound_length; column++) {
          /*!for values from y = half_length TO y = half_length +BOUND_LENGTH */
          /*! and for x = half_length to x = half_length + BOUND_LENGTH */
          next[(depth + start_y) * nz_nx + row * nx + column + start_x] *=
              min(this->sponge_coeffs[column], this->sponge_coeffs[depth]);
          /*!for values from y = ny-half_length TO y =
           * ny-half_length-BOUND_LENGTH*/
          /*! and for x = half_length to x = half_length + BOUND_LENGTH */
          next[(end_y - 1 - depth) * nz_nx + row * nx + column + start_x] *=
              min(this->sponge_coeffs[column], this->sponge_coeffs[depth]);
          /*!for values from y = half_length TO y = half_length +BOUND_LENGTH */
          /*! and for x = nx-half_length to x = nx-half_length - BOUND_LENGTH */
          next[(depth + start_y) * nz_nx + row * nx + (end_x - 1 - column)] *=
              min(this->sponge_coeffs[column], this->sponge_coeffs[depth]);
          /*!for values from y = ny-half_length TO y =
           * ny-half_length-BOUND_LENGTH*/
          /*! and for x = nx-half_length to x = nx - half_length - BOUND_LENGTH
           */
          next[(end_y - 1 - depth) * nz_nx + row * nx + (end_x - 1 - column)] *=
              min(this->sponge_coeffs[column], this->sponge_coeffs[depth]);
        }
      }
    }
    }
  }
}

void SpongeBoundaryManager ::ApplyBoundary(uint kernel_id) {
  if (kernel_id == 0) {
    ApplyBoundaryOnField(grid->pressure_current);
  }
}

void SpongeBoundaryManager ::ExtendModel() {
  for (auto const &extension : this->extensions) {
    extension->ExtendProperty();
  }
}

void SpongeBoundaryManager ::ReExtendModel() {
  for (auto const &extension : this->extensions) {
    extension->ReExtendProperty();
  }
}

SpongeBoundaryManager ::~SpongeBoundaryManager() {
  for (auto const &extension : this->extensions) {
    delete extension;
  }
  this->extensions.clear();
  delete[] this->sponge_coeffs;
}

void SpongeBoundaryManager::SetComputationParameters(
    ComputationParameters *parameters) {
  for (auto const &extension : this->extensions) {
    extension->SetHalfLength(parameters->half_length);
    extension->SetBoundaryLength(parameters->boundary_length);
  }
  this->parameters = parameters;
  uint bound_length = parameters->boundary_length;
  this->sponge_coeffs = new float[bound_length];
  for (uint i = 0; i < bound_length; i++) {
    this->sponge_coeffs[i] = calculation(i);
  }
}

void SpongeBoundaryManager::SetGridBox(GridBox *grid_box) {
  this->extensions[0]->SetGridBox(grid_box);
  this->extensions[0]->SetProperty(grid_box->velocity, grid_box->window_velocity);
  if (this->is_staggered) {
    StaggeredGrid *grid = (StaggeredGrid *)grid_box;
    this->extensions[1]->SetGridBox(grid);
    this->extensions[1]->SetProperty(grid->density, grid->window_density);
  }
  this->grid = grid_box;
}

void SpongeBoundaryManager::AdjustModelForBackward() {
  for (auto const &extension : this->extensions) {
    extension->AdjustPropertyForBackward();
  }
}
