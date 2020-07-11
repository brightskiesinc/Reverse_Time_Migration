#include "staggered_computation_kernel.h"
#include <cmath>
#include <float.h>
#include <fstream>
#include <iostream>
#include <limits.h>
#include <math.h>
#include <skeleton/helpers/timer/timer.hpp>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

using namespace std;

StaggeredComputationKernel::StaggeredComputationKernel(bool is_forward) {
  this->is_forward = is_forward;
  this->boundary_manager = nullptr;
}

StaggeredComputationKernel::~StaggeredComputationKernel() = default;

template <bool is_forward, bool is_2D, HALF_LENGTH half_length>
void Computation(StaggeredGrid *grid,
                 AcousticOmpComputationParameters *parameters,
                 BoundaryManager *boundary_manager) {
  // Read parameters into local variables to be shared.
  float *curr_base = grid->pressure_current;
  float *next_base = grid->pressure_next;
  float *particle_vel_x = grid->particle_velocity_x_current;
  float *particle_vel_y = grid->particle_velocity_y_current;
  float *particle_vel_z = grid->particle_velocity_z_current;
  float *vel_base = grid->window_velocity;
  float *density_base = grid->window_density;
  int wnx = grid->window_size.window_nx;
  int wny = grid->window_size.window_ny;
  int wnz = grid->window_size.window_nz;
  float dx = grid->cell_dimensions.dx;
  float dy;
  float dz = grid->cell_dimensions.dz;
  float dt = grid->dt;
  float *coeff = parameters->first_derivative_staggered_fd_coeff;
  int block_x = parameters->block_x;
  int block_y = parameters->block_y;
  int block_z = parameters->block_z;
  int nxEnd = wnx - half_length;
  int nyEnd = 1;
  int nzEnd = wnz - half_length;
  int wnxnz = wnx * wnz;
  int nx = wnx;
  int nz = wnz;
  int nxnz = nx * nz;
  int size = (wnx - 2 * half_length) * (wnz - 2 * half_length);

  // General note: floating point operations for forward is the same as backward
  // (calculated below are for forward). number of floating point operations for
  // the pressure kernel in 2D for the half_length loop:6*k -2(for adding zeros)
  // =6*K-2 where K is the half_length 5 floating point operations outside the
  // half_length loop Total = 6*K-2+5 =6*K+3
  int flops_per_pressure = 6 * half_length + 3;

  // vel,curr,next,vel_x,vel_z
  int num_of_arrays_pressure = 5;

  // General note: floating point operations for forward is the same as backward
  // (calculated below are for forward). number of floating point operations for
  // the velocity kernel in 2D for the half_length loop:6*k -2(for adding zeros)
  // =6*K-2 where K is the half_length 6 floating point operations outside the
  // half_length loop Total = 6*K-2+6 =6*K+4
  int flops_per_velocity = 6 * half_length + 4;

  // curr,den,vel_x(load),vel_x(store),vel_z(load),vel_z(store)
  int num_of_arrays_velocity = 6;

  int y_start = 0;
  if (!is_2D) {
    dy = grid->cell_dimensions.dy;
    y_start = half_length;
    nyEnd = wny - half_length;

    // General note: floating point operations for forward is the same as
    // backward (calculated below are for forward). number of floating point
    // operations for the pressure kernel in 3D for the half_length loop:9*k
    // -3(for adding zeros) =9*K-3 where K is the half_length 7 floating point
    // operations outside the half_length loop Total = 9*K-3+7 =9*K+4
    flops_per_pressure = 9 * half_length + 4;

    // vel,curr,next,vel_x,vel_z,vel_y
    num_of_arrays_pressure = 6;

    // General note: floating point operations for forward is the same as
    // backward (calculated below are for forward). number of floating point
    // operations for the velocity kernel in 3D for the half_length loop:9*k
    // -3(for adding zeros) =9*K-3 where K is the half_length 9 floating point
    // operations outside the half_length loop Total = 9*K-3+9 =9*K+6
    flops_per_velocity = 9 * half_length + 6;

    // curr,den,vel_x(load),vel_x(store),vel_z(load),vel_z(store),vel_y(load),vel_y(store)
    num_of_arrays_velocity = 8;
  }
  // Pre-compute the coefficients for each direction.
  float coeff_x[half_length];
  float coeff_y[half_length];
  float coeff_z[half_length];
  int vertical[half_length];
  int front[half_length];
  for (int i = 0; i < half_length; i++) {
    coeff_x[i] = coeff[i + 1];
    coeff_z[i] = coeff[i + 1];
    vertical[i] = (i + 1) * wnx;
    if (!is_2D) {
      coeff_y[i] = coeff[i + 1];
      front[i] = (i + 1) * wnxnz;
    }
  }

  // start the timers for the velocity kernel.
  Timer *timer = Timer::getInstance();
  // timer->start_timer("ComputationKernel:velocity kernel");
  timer->_start_timer_for_kernel("ComputationKernel::velocity kernel", size,
                                 num_of_arrays_velocity, true,
                                 flops_per_velocity);
// Start the computation by creating the threads.
#pragma omp parallel default(shared)
  {
    float *curr, *next, *den, *vel, *vel_x, *vel_y, *vel_z;
    // Three loops for cache blocking : utilizing the cache to the maximum to
    // speed up computation.
#pragma omp for schedule(static, 1) collapse(2)
    for (int by = y_start; by < nyEnd; by += block_y) {
      for (int bz = half_length; bz < nzEnd; bz += block_z) {
        for (int bx = half_length; bx < nxEnd; bx += block_x) {
          // Calculate the endings appropriately (Handle remainder of the cache
          // blocking loops).
          int ixEnd = fmin(block_x, nxEnd - bx);
          int izEnd = fmin(bz + block_z, nzEnd);
          int iyEnd = fmin(by + block_y, nyEnd);
          // Loop on the elements in the block.
          for (int iy = by; iy < iyEnd; ++iy) {
            for (int iz = bz; iz < izEnd; ++iz) {
              // Pre-compute in advance the pointer to the start of the current
              // start point of the processing.
              int offset = iy * wnxnz + iz * wnx + bx;
              // Velocity moves with the full nx and nz not the windows ones.
              curr = curr_base + offset;
              den = density_base + offset;
              vel_x = particle_vel_x + offset;
              vel_y = particle_vel_y + offset;
              vel_z = particle_vel_z + offset;
#pragma vector aligned
#pragma vector vecremainder
#pragma omp simd
#pragma ivdep
              for (int ix = 0; ix < ixEnd; ++ix) {
                float value_x = 0;
                float value_y = 0;
                float value_z = 0;
                for (int i = 0; i < half_length; ++i) {
                  // 3 floating point operations
                  value_x += coeff_x[i] * (curr[ix + (i + 1)] - curr[ix - i]);
                  // 3 floating point operations
                  value_z += coeff_z[i] *
                             (curr[ix + (i + 1) * nx] - curr[ix - (i * nx)]);
                  if (!is_2D) {
                    // 3 floating point operations
                    value_y += coeff_y[i] * (curr[ix + (i + 1) * nxnz] -
                                             curr[ix - (i * nxnz)]);
                  }
                }
                if (is_forward) {
                  // 3 floating point operations
                  vel_x[ix] = vel_x[ix] - (den[ix] / dx) * value_x;
                  if (!is_2D) {
                    // 3 floating point operations
                    vel_y[ix] = vel_y[ix] - (den[ix] / dy) * value_y;
                  }
                  // 3 floating point operations
                  vel_z[ix] = vel_z[ix] - (den[ix] / dz) * value_z;
                } else {
                  vel_x[ix] = vel_x[ix] + (den[ix] / dx) * value_x;
                  if (!is_2D) {
                    vel_y[ix] = vel_y[ix] + (den[ix] / dy) * value_y;
                  }
                  vel_z[ix] = vel_z[ix] + (den[ix] / dz) * value_z;
                }
              }
            }
          }
        }
      }
    }
  }

  // the end of time of particle velocity kernel
  timer->stop_timer("ComputationKernel::velocity kernel");
  timer->start_timer("BoundaryManager::ApplyBoundary(Velocity)");
  if (boundary_manager != nullptr) {
    boundary_manager->ApplyBoundary(1);
  }
  timer->stop_timer("BoundaryManager::ApplyBoundary(Velocity)");
  // start the timer of the pressure kernel
  timer->_start_timer_for_kernel("ComputationKernel::pressure kernel", size,
                                 num_of_arrays_pressure, true,
                                 flops_per_pressure);
#pragma omp parallel default(shared)
  {
    float *curr, *next, *den, *vel, *vel_x, *vel_y, *vel_z;
    // Pressure Calculation
#pragma omp for schedule(static, 1) collapse(2)
    for (int by = y_start; by < nyEnd; by += block_y) {
      for (int bz = half_length; bz < nzEnd; bz += block_z) {
        for (int bx = half_length; bx < nxEnd; bx += block_x) {
          // Calculate the endings appropriately (Handle remainder of the cache
          // blocking loops).
          int ixEnd = fmin(block_x, nxEnd - bx);
          int izEnd = fmin(bz + block_z, nzEnd);
          int iyEnd = fmin(by + block_y, nyEnd);
          // Loop on the elements in the block.
          for (int iy = by; iy < iyEnd; ++iy) {
            for (int iz = bz; iz < izEnd; ++iz) {
              // Pre-compute in advance the pointer to the start of the current
              // start point of the processing.
              int offset = iy * wnxnz + iz * wnx + bx;
              // Velocity moves with the full nx and nz not the windows ones.
              vel = vel_base + offset;
              curr = curr_base + offset;
              next = next_base + offset;
              vel_x = particle_vel_x + offset;
              vel_y = particle_vel_y + offset;
              vel_z = particle_vel_z + offset;
#pragma vector aligned
#pragma vector vecremainder
#pragma omp simd
#pragma ivdep
              for (int ix = 0; ix < ixEnd; ++ix) {
                float value_x = 0;
                float value_y = 0;
                float value_z = 0;
                for (int i = 0; i < half_length; ++i) {
                  // 3 floating point operations
                  value_x += coeff_x[i] * (vel_x[ix + i] - vel_x[ix - (i + 1)]);
                  // 3 floating point operations
                  value_z += coeff_z[i] *
                             (vel_z[ix + i * nx] - vel_z[ix - ((i + 1) * nx)]);
                  if (!is_2D) {
                    // 3 floating point operations
                    value_y += coeff_y[i] * (vel_y[ix + i * nxnz] -
                                             vel_y[ix - ((i + 1) * nxnz)]);
                  }
                }
                if (is_forward) {
                  if (!is_2D) {
                    // 7 floating point operations
                    next[ix] =
                        curr[ix] - vel[ix] * ((value_x / dx) + (value_z / dz) +
                                              (value_y / dy));
                  } else {
                    // 5 floating point operations
                    next[ix] =
                        curr[ix] - vel[ix] * ((value_x / dx) + (value_z / dz));
                  }
                } else {
                  if (!is_2D) {
                    next[ix] =
                        curr[ix] + vel[ix] * ((value_x / dx) + (value_z / dz) +
                                              (value_y / dy));
                  } else {
                    next[ix] =
                        curr[ix] + vel[ix] * ((value_x / dx) + (value_z / dz));
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  timer->stop_timer("ComputationKernel::pressure kernel");
}

template void
Computation<true, false, O_2>(StaggeredGrid *grid,
                              AcousticOmpComputationParameters *parameters,
                              BoundaryManager *boundary_manager);
template void
Computation<true, false, O_4>(StaggeredGrid *grid,
                              AcousticOmpComputationParameters *parameters,
                              BoundaryManager *boundary_manager);
template void
Computation<true, false, O_8>(StaggeredGrid *grid,
                              AcousticOmpComputationParameters *parameters,
                              BoundaryManager *boundary_manager);
template void
Computation<true, false, O_12>(StaggeredGrid *grid,
                               AcousticOmpComputationParameters *parameters,
                               BoundaryManager *boundary_manager);
template void
Computation<true, false, O_16>(StaggeredGrid *grid,
                               AcousticOmpComputationParameters *parameters,
                               BoundaryManager *boundary_manager);
template void
Computation<true, true, O_2>(StaggeredGrid *grid,
                             AcousticOmpComputationParameters *parameters,
                             BoundaryManager *boundary_manager);
template void
Computation<true, true, O_4>(StaggeredGrid *grid,
                             AcousticOmpComputationParameters *parameters,
                             BoundaryManager *boundary_manager);
template void
Computation<true, true, O_8>(StaggeredGrid *grid,
                             AcousticOmpComputationParameters *parameters,
                             BoundaryManager *boundary_manager);
template void
Computation<true, true, O_12>(StaggeredGrid *grid,
                              AcousticOmpComputationParameters *parameters,
                              BoundaryManager *boundary_manager);
template void
Computation<true, true, O_16>(StaggeredGrid *grid,
                              AcousticOmpComputationParameters *parameters,
                              BoundaryManager *boundary_manager);
template void
Computation<false, false, O_2>(StaggeredGrid *grid,
                               AcousticOmpComputationParameters *parameters,
                               BoundaryManager *boundary_manager);
template void
Computation<false, false, O_4>(StaggeredGrid *grid,
                               AcousticOmpComputationParameters *parameters,
                               BoundaryManager *boundary_manager);
template void
Computation<false, false, O_8>(StaggeredGrid *grid,
                               AcousticOmpComputationParameters *parameters,
                               BoundaryManager *boundary_manager);
template void
Computation<false, false, O_12>(StaggeredGrid *grid,
                                AcousticOmpComputationParameters *parameters,
                                BoundaryManager *boundary_manager);
template void
Computation<false, false, O_16>(StaggeredGrid *grid,
                                AcousticOmpComputationParameters *parameters,
                                BoundaryManager *boundary_manager);
template void
Computation<false, true, O_2>(StaggeredGrid *grid,
                              AcousticOmpComputationParameters *parameters,
                              BoundaryManager *boundary_manager);
template void
Computation<false, true, O_4>(StaggeredGrid *grid,
                              AcousticOmpComputationParameters *parameters,
                              BoundaryManager *boundary_manager);
template void
Computation<false, true, O_8>(StaggeredGrid *grid,
                              AcousticOmpComputationParameters *parameters,
                              BoundaryManager *boundary_manager);
template void
Computation<false, true, O_12>(StaggeredGrid *grid,
                               AcousticOmpComputationParameters *parameters,
                               BoundaryManager *boundary_manager);
template void
Computation<false, true, O_16>(StaggeredGrid *grid,
                               AcousticOmpComputationParameters *parameters,
                               BoundaryManager *boundary_manager);

void StaggeredComputationKernel::Step() {
  // Take a step in time.
  if (is_forward) {
    if ((grid->grid_size.ny) == 1) {
      switch (parameters->half_length) {
      case O_2:
        Computation<true, true, O_2>(grid, parameters, this->boundary_manager);
        break;
      case O_4:
        Computation<true, true, O_4>(grid, parameters, this->boundary_manager);
        break;
      case O_8:
        Computation<true, true, O_8>(grid, parameters, this->boundary_manager);
        break;
      case O_12:
        Computation<true, true, O_12>(grid, parameters, this->boundary_manager);
        break;
      case O_16:
        Computation<true, true, O_16>(grid, parameters, this->boundary_manager);
        break;
      }
    } else {
      switch (parameters->half_length) {
      case O_2:
        Computation<true, false, O_2>(grid, parameters, this->boundary_manager);
        break;
      case O_4:
        Computation<true, false, O_4>(grid, parameters, this->boundary_manager);
        break;
      case O_8:
        Computation<true, false, O_8>(grid, parameters, this->boundary_manager);
        break;
      case O_12:
        Computation<true, false, O_12>(grid, parameters,
                                       this->boundary_manager);
        break;
      case O_16:
        Computation<true, false, O_16>(grid, parameters,
                                       this->boundary_manager);
        break;
      }
    }
  } else {
    if ((grid->grid_size.ny) == 1) {
      switch (parameters->half_length) {
      case O_2:
        Computation<false, true, O_2>(grid, parameters, this->boundary_manager);
        break;
      case O_4:
        Computation<false, true, O_4>(grid, parameters, this->boundary_manager);
        break;
      case O_8:
        Computation<false, true, O_8>(grid, parameters, this->boundary_manager);
        break;
      case O_12:
        Computation<false, true, O_12>(grid, parameters,
                                       this->boundary_manager);
        break;
      case O_16:
        Computation<false, true, O_16>(grid, parameters,
                                       this->boundary_manager);
        break;
      }
    } else {
      switch (parameters->half_length) {
      case O_2:
        Computation<false, false, O_2>(grid, parameters,
                                       this->boundary_manager);
        break;
      case O_4:
        Computation<false, false, O_4>(grid, parameters,
                                       this->boundary_manager);
        break;
      case O_8:
        Computation<false, false, O_8>(grid, parameters,
                                       this->boundary_manager);
        break;
      case O_12:
        Computation<false, false, O_12>(grid, parameters,
                                        this->boundary_manager);
        break;
      case O_16:
        Computation<false, false, O_16>(grid, parameters,
                                        this->boundary_manager);
        break;
      }
    }
  }
  float *temp = grid->pressure_current;
  grid->pressure_current = grid->pressure_next;
  grid->pressure_next = temp;
  Timer *timer = Timer::getInstance();
  timer->start_timer("BoundaryManager::ApplyBoundary(Pressure)");
  if (this->boundary_manager != nullptr) {
    this->boundary_manager->ApplyBoundary(0);
  }
  timer->stop_timer("BoundaryManager::ApplyBoundary(Pressure)");
}

void StaggeredComputationKernel::FirstTouch(float *ptr, uint nx, uint nz,
                                            uint ny) {
  float *curr_base = ptr;
  float *curr;
  int wnx = nx;
  int wny = ny;
  int wnz = nz;
  int wnxnz = wnx * wnz;
  int nxEnd = wnx - parameters->half_length;
  int nyEnd = 1;
  int nzEnd = wnz - parameters->half_length;
  int y_start = 0;
  if (ny > 1) {
    y_start = parameters->half_length;
    nyEnd = wny - parameters->half_length;
  }
  uint half_length = parameters->half_length;
  int block_x = parameters->block_x;
  int block_y = parameters->block_y;
  int block_z = parameters->block_z;
  Timer *timer = Timer::getInstance();
  timer->start_timer("ComputationKernel::FirstTouch");
  // First touch : access elements in the same way used in the computation
  // kernel step.
#pragma omp parallel for schedule(static, 1) collapse(3)
  for (int by = y_start; by < nyEnd; by += block_y) {
    for (int bz = half_length; bz < nzEnd; bz += block_z) {
      for (int bx = half_length; bx < nxEnd; bx += block_x) {
        int izEnd = fmin(bz + block_z, nzEnd);
        int iyEnd = fmin(by + block_y, nyEnd);
        int ixEnd = fmin(block_x, nxEnd - bx);
        for (int iy = by; iy < iyEnd; ++iy) {
          for (int iz = bz; iz < izEnd; ++iz) {
            curr = curr_base + iy * wnxnz + iz * wnx + bx;
#pragma ivdep
            for (int ix = 0; ix < ixEnd; ++ix) {
              curr[ix] = 0;
            }
          }
        }
      }
    }
  }
  timer->stop_timer("ComputationKernel::FirstTouch");
}
void StaggeredComputationKernel::SetComputationParameters(
    ComputationParameters *parameters) {
  this->parameters = (AcousticOmpComputationParameters *)(parameters);
  if (this->parameters == nullptr) {
    std::cout << "Not a compatible computation parameters : "
                 "expected AcousticOmpComputationParameters"
              << std::endl;
    exit(-1);
  }
}

void StaggeredComputationKernel::SetGridBox(GridBox *grid_box) {
  ;
  this->grid = (StaggeredGrid *)(grid_box);
  if (this->grid == nullptr) {
    std::cout << "Not a compatible gridbox : "
                 "expected AcousticSecondGrid"
              << std::endl;
    exit(-1);
  }
}
