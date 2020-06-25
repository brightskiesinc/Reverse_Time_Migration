//
// Created by mirnamoawad on 1/20/20.
//
// clang-format off
#include <rtm-framework/skeleton/base/datatypes.h>
#include "boundary_saver.h"
// clang-format on

void SaveBoundaries(GridBox *main_grid, ComputationParameters *parameters,
                    float *backup_boundaries, uint step, uint boundarysize) {
  uint index = 0;
  uint size_of_boundaries = boundarysize;
  uint time_step = step;
  uint half_length = parameters->half_length;
  uint bound_length = parameters->boundary_length;
  uint offset = half_length + bound_length;
  uint start_y = 0;
  uint end_y = 1;
  uint ny = main_grid->grid_size.ny;
  uint wnx = main_grid->window_size.window_nx;
  uint wnz = main_grid->window_size.window_nz;
  uint wny = main_grid->window_size.window_ny;
  uint start_z = offset;
  uint end_z = wnz - offset;
  uint start_x = offset;
  uint end_x = wnx - offset;
  uint wnznx = wnx * wnz;

  if (ny > 1) {
    start_y = offset;
    end_y = wny;
  }
  for (int iy = start_y; iy < end_y; iy++) {
    for (int iz = start_z; iz < end_z; iz++) {
      for (int ix = 0; ix < half_length; ix++) {
        backup_boundaries[time_step * size_of_boundaries + index] =
            main_grid
                ->pressure_current[iy * wnznx + iz * wnx + bound_length + ix];
        index++;
        backup_boundaries[time_step * size_of_boundaries + index] =
            main_grid->pressure_current[iy * wnznx + iz * wnx +
                                        (wnx - bound_length - 1) - ix];
        index++;
      }
    }
  }
  for (int iy = start_y; iy < end_y; iy++) {
    for (int iz = 0; iz < half_length; iz++) {
      for (int ix = start_x; ix < end_x; ix++) {
        backup_boundaries[time_step * size_of_boundaries + index] =
            main_grid
                ->pressure_current[iy * wnznx + (bound_length + iz) * wnx + ix];
        index++;
        backup_boundaries[time_step * size_of_boundaries + index] =
            main_grid
                ->pressure_current[iy * wnznx +
                                   (wnz - bound_length - 1 - iz) * wnx + ix];
        index++;
      }
    }
  }
  if (ny > 1) {
    for (int iy = 0; iy < half_length; iy++) {
      for (int iz = start_z; iz < end_z; iz++) {
        for (int ix = start_x; ix < end_x; ix++) {
          backup_boundaries[time_step * size_of_boundaries + index] =
              main_grid->pressure_current[(bound_length + iy) * wnznx +
                                          iz * wnx + ix];
          index++;
          backup_boundaries[time_step * size_of_boundaries + index] =
              main_grid
                  ->pressure_current[(wny - bound_length - 1 - iy) * wnznx +
                                     iz * wnx + ix];
          index++;
        }
      }
    }
  }
}

void RestoreBoundaries(GridBox *main_grid, GridBox *internal_grid,
                       ComputationParameters *parameters,
                       float *backup_boundaries, uint step, uint boundarysize) {
  uint index = 0;
  uint size_of_boundaries = boundarysize;
  uint time_step = step;
  uint half_length = parameters->half_length;
  uint bound_length = parameters->boundary_length;
  uint offset = half_length + bound_length;
  uint start_y = 0;
  uint end_y = 1;
  uint ny = main_grid->grid_size.ny;
  uint wnx = main_grid->window_size.window_nx;
  uint wnz = main_grid->window_size.window_nz;
  uint wny = main_grid->window_size.window_ny;
  uint start_z = offset;
  uint end_z = wnz - offset;
  uint start_x = offset;
  uint end_x = wnx - offset;
  uint wnznx = wnx * wnz;
  if (ny > 1) {
    start_y = offset;
    end_y = wny;
  }
  for (int iy = start_y; iy < end_y; iy++) {
    for (int iz = start_z; iz < end_z; iz++) {
      for (int ix = 0; ix < half_length; ix++) {
        internal_grid
            ->pressure_current[iy * wnznx + iz * wnx + bound_length + ix] =
            backup_boundaries[time_step * size_of_boundaries + index];
        index++;
        internal_grid->pressure_current[iy * wnznx + iz * wnx +
                                        (wnx - bound_length - 1) - ix] =
            backup_boundaries[time_step * size_of_boundaries + index];
        index++;
      }
    }
  }
  for (int iy = start_y; iy < end_y; iy++) {
    for (int iz = 0; iz < half_length; iz++) {
      for (int ix = start_x; ix < end_x; ix++) {
        internal_grid
            ->pressure_current[iy * wnznx + (bound_length + iz) * wnx + ix] =
            backup_boundaries[time_step * size_of_boundaries + index];
        index++;
        internal_grid
            ->pressure_current[iy * wnznx +
                               (wnz - bound_length - 1 - iz) * wnx + ix] =
            backup_boundaries[time_step * size_of_boundaries + index];
        index++;
      }
    }
  }
  if (ny > 1) {
    for (int iy = 0; iy < half_length; iy++) {
      for (int iz = start_z; iz < end_z; iz++) {
        for (int ix = start_x; ix < end_x; ix++) {
          internal_grid
              ->pressure_current[(bound_length + iy) * wnznx + iz * wnx + ix] =
              backup_boundaries[time_step * size_of_boundaries + index];
          index++;
          internal_grid
              ->pressure_current[(wny - bound_length - 1 - iy) * wnznx +
                                 iz * wnx + ix] =
              backup_boundaries[time_step * size_of_boundaries + index];
          index++;
        }
      }
    }
  }
}
