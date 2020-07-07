#include "second_order_computation_kernel.h"
#include <cmath>
#include <iostream>
#include <skeleton/helpers/timer/timer.hpp>

#define fma(a, b, c) (a) * (b) + (c)

SecondOrderComputationKernel::~SecondOrderComputationKernel() = default;

using namespace std;

SecondOrderComputationKernel::SecondOrderComputationKernel() {
  this->boundary_manager = nullptr;
}
template <bool is_2D, HALF_LENGTH half_length>
void Computation(AcousticSecondGrid *grid,
                 AcousticOmpComputationParameters *parameters) {
  // Read parameters into local variables to be shared.
  float *prev_base = grid->pressure_previous;
  float *curr_base = grid->pressure_current;
  float *next_base = grid->pressure_next;
  float *vel_base = grid->velocity;
  int wnx = grid->window_size.window_nx;
  int wny = grid->window_size.window_ny;
  int wnz = grid->window_size.window_nz;
  float dx = grid->cell_dimensions.dx;
  float dy;
  float dz = grid->cell_dimensions.dz;
  int nx = grid->grid_size.nx;
  int nz = grid->grid_size.nz;
  float dx2 = 1 / (dx * dx);
  float dy2;
  float dz2 = 1 / (dz * dz);
  float *coeff = parameters->second_derivative_fd_coeff;
  int block_x = parameters->block_x;
  int block_y = parameters->block_y;
  int block_z = parameters->block_z;
  int nxEnd = wnx - half_length;
  int nyEnd = 1;
  int nzEnd = wnz - half_length;
  int wnxnz = wnx * wnz;
  int nxnz = nx * nz;
  int size = (wnx - 2 * half_length) * (wnz - 2 * half_length);

  // General note: floating point operations for forward is the same as backward
  // (calculated below are for forward). number of floating point operations for
  // the computation kernel in 2D for the half_length loop:6*k,where K is the
  // half_length 5 floating point operations outside the half_length loop Total
  // = 6*K+5 =6*K+5
  int flops_per_second = 6 * half_length + 5;
  //operate on the window velocity(numa consistency ensured).
  vel_base = grid->window_velocity;
  int y_start = 0;
  if (!is_2D) {
    dy = grid->cell_dimensions.dy;
    dy2 = 1 / (dy * dy);
    y_start = half_length;
    nyEnd = wny - half_length;

    // General note: floating point operations for forward is the same as
    // backward (calculated below are for forward). number of floating point
    // operations for the computation kernel in 3D for the half_length loop:9*K
    // where K is the half_length 5 floating point operations outside the
    // half_length loop Total = 9*K+5 =9*K+5
    flops_per_second = 9 * half_length + 5;
  }
  // Pre-compute the coefficients for each direction.
  float coeff_x[half_length];
  float coeff_y[half_length];
  float coeff_z[half_length];
  int vertical[half_length];
  int front[half_length];
  float coeff_xyz;
  for (int i = 0; i < half_length; i++) {
    coeff_x[i] = coeff[i + 1] * dx2;
    coeff_z[i] = coeff[i + 1] * dz2;
    vertical[i] = (i + 1) * wnx;
    if (!is_2D) {
      coeff_y[i] = coeff[i + 1] * dy2;
      front[i] = (i + 1) * wnxnz;
    }
  }
  if (is_2D) {
    coeff_xyz = coeff[0] * (dx2 + dz2);
  } else {
    coeff_xyz = coeff[0] * (dx2 + dy2 + dz2);
  }

  Timer *timer = Timer::getInstance();
  timer->_start_timer_for_kernel("ComputationKernel::kernel", size, 4, true,
                                 flops_per_second);

  // Start the computation by creating the threads.
#pragma omp parallel default(shared)
  {

    float *prev, *curr, *next, *vel;
// Three loops for cache blocking : utilizing the cache to the maximum to speed
// up computation.
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
              // Pre-compute and advance the pointer to the start of the current
              // start point of the processing.
              int offset = iy * wnxnz + iz * wnx + bx;
              // Velocity moves with the full nx and nz not the windows ones.
              vel = vel_base + offset;
              prev = prev_base + offset;
              curr = curr_base + offset;
              next = next_base + offset;

#pragma vector aligned
#pragma vector vecremainder
#pragma omp simd
#pragma ivdep
              for (int ix = 0; ix < ixEnd; ++ix) {
                // Calculate the finite difference using sequence of fma
                // instructions.
                float value = 0;
                // 1 floating point operation
                value = fma(curr[ix], coeff_xyz, value);
                // Calculate Finite Difference in the x-direction.
                // 3 floating point operations
                value = fma(curr[ix - 1] + curr[ix + 1], coeff_x[0], value);
                if (half_length > 1) {
                  value = fma(curr[ix - 2] + curr[ix + 2], coeff_x[1], value);
                }
                if (half_length > 2) {
                  value = fma(curr[ix - 3] + curr[ix + 3], coeff_x[2], value);
                  value = fma(curr[ix - 4] + curr[ix + 4], coeff_x[3], value);
                }
                if (half_length > 4) {
                  value = fma(curr[ix - 5] + curr[ix + 5], coeff_x[4], value);
                  value = fma(curr[ix - 6] + curr[ix + 6], coeff_x[5], value);
                }
                if (half_length > 6) {
                  value = fma(curr[ix - 7] + curr[ix + 7], coeff_x[6], value);
                  value = fma(curr[ix - 8] + curr[ix + 8], coeff_x[7], value);
                }
                // Calculate Finite Difference in the z-direction.
                // 3 floating point operations
                value = fma(curr[ix - vertical[0]] + curr[ix + vertical[0]],
                            coeff_z[0], value);
                if (half_length > 1) {
                  value = fma(curr[ix - vertical[1]] + curr[ix + vertical[1]],
                              coeff_z[1], value);
                }
                if (half_length > 2) {
                  value = fma(curr[ix - vertical[2]] + curr[ix + vertical[2]],
                              coeff_z[2], value);
                  value = fma(curr[ix - vertical[3]] + curr[ix + vertical[3]],
                              coeff_z[3], value);
                }
                if (half_length > 4) {
                  value = fma(curr[ix - vertical[4]] + curr[ix + vertical[4]],
                              coeff_z[4], value);
                  value = fma(curr[ix - vertical[5]] + curr[ix + vertical[5]],
                              coeff_z[5], value);
                }
                if (half_length > 6) {
                  value = fma(curr[ix - vertical[6]] + curr[ix + vertical[6]],
                              coeff_z[6], value);
                  value = fma(curr[ix - vertical[7]] + curr[ix + vertical[7]],
                              coeff_z[7], value);
                }
                if (!is_2D) {
                  // Calculate Finite Difference in the y-direction.
                  // 3 floating point operations
                  value = fma(curr[ix - front[0]] + curr[ix + front[0]],
                              coeff_y[0], value);
                  if (half_length > 1) {
                    value = fma(curr[ix - front[1]] + curr[ix + front[1]],
                                coeff_y[1], value);
                  }
                  if (half_length > 2) {
                    value = fma(curr[ix - front[2]] + curr[ix + front[2]],
                                coeff_y[2], value);
                    value = fma(curr[ix - front[3]] + curr[ix + front[3]],
                                coeff_y[3], value);
                  }
                  if (half_length > 4) {
                    value = fma(curr[ix - front[4]] + curr[ix + front[4]],
                                coeff_y[4], value);
                    value = fma(curr[ix - front[5]] + curr[ix + front[5]],
                                coeff_y[5], value);
                  }
                  if (half_length > 6) {
                    value = fma(curr[ix - front[6]] + curr[ix + front[6]],
                                coeff_y[6], value);
                    value = fma(curr[ix - front[7]] + curr[ix + front[7]],
                                coeff_y[7], value);
                  }
                }
                // Calculate the next pressure value according to the second
                // order acoustic wave equation.
                // 4 floating point operations
                next[ix] = (2 * curr[ix]) - prev[ix] + (vel[ix] * value);
              }
            }
          }
        }
      }
    }
  }
  timer->stop_timer("ComputationKernel::kernel");
}

template void
Computation<false, O_2>(AcousticSecondGrid *grid,
                        AcousticOmpComputationParameters *parameters);
template void
Computation<false, O_4>(AcousticSecondGrid *grid,
                        AcousticOmpComputationParameters *parameters);
template void
Computation<false, O_8>(AcousticSecondGrid *grid,
                        AcousticOmpComputationParameters *parameters);
template void
Computation<false, O_12>(AcousticSecondGrid *grid,
                         AcousticOmpComputationParameters *parameters);
template void
Computation<false, O_16>(AcousticSecondGrid *grid,
                         AcousticOmpComputationParameters *parameters);
template void
Computation<true, O_2>(AcousticSecondGrid *grid,
                       AcousticOmpComputationParameters *parameters);
template void
Computation<true, O_4>(AcousticSecondGrid *grid,
                       AcousticOmpComputationParameters *parameters);
template void
Computation<true, O_8>(AcousticSecondGrid *grid,
                       AcousticOmpComputationParameters *parameters);
template void
Computation<true, O_12>(AcousticSecondGrid *grid,
                        AcousticOmpComputationParameters *parameters);
template void
Computation<true, O_16>(AcousticSecondGrid *grid,
                        AcousticOmpComputationParameters *parameters);

void SecondOrderComputationKernel::Step() {
  // Take a step in time.
  if ((grid->grid_size.ny) == 1) {
    switch (parameters->half_length) {
    case O_2:
      Computation<true, O_2>(grid, parameters);
      break;
    case O_4:
      Computation<true, O_4>(grid, parameters);
      break;
    case O_8:
      Computation<true, O_8>(grid, parameters);
      break;
    case O_12:
      Computation<true, O_12>(grid, parameters);
      break;
    case O_16:
      Computation<true, O_16>(grid, parameters);
      break;
    }
  } else {
    switch (parameters->half_length) {
    case O_2:
      Computation<false, O_2>(grid, parameters);
      break;
    case O_4:
      Computation<false, O_4>(grid, parameters);
      break;
    case O_8:
      Computation<false, O_8>(grid, parameters);
      break;
    case O_12:
      Computation<false, O_12>(grid, parameters);
      break;
    case O_16:
      Computation<false, O_16>(grid, parameters);
      break;
    }
  }
  // Swap pointers : Next to current, current to prev and unwanted prev to next
  // to be overwritten.
  if (grid->pressure_previous == grid->pressure_next) {
    // two pointers case : curr becomes both next and prev, while next becomes
    // current.
    grid->pressure_previous = grid->pressure_current;
    grid->pressure_current = grid->pressure_next;
    grid->pressure_next = grid->pressure_previous;
  } else {
    // three pointers : normal swapping between the three pointers.
    float *temp = grid->pressure_next;
    grid->pressure_next = grid->pressure_previous;
    grid->pressure_previous = grid->pressure_current;
    grid->pressure_current = temp;
  }
  Timer *timer = Timer::getInstance();
  timer->start_timer("BoundaryManager::ApplyBoundary");
  if (this->boundary_manager != nullptr) {
    this->boundary_manager->ApplyBoundary();
  }
  timer->stop_timer("BoundaryManager::ApplyBoundary");
}

void SecondOrderComputationKernel::FirstTouch(float *ptr, uint nx, uint nz,
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
  // First touch : access elements in the same way used in the computation
  // kernel step.

  Timer *timer = Timer::getInstance();
  timer->start_timer("ComputationKernel::FirstTouch");
#pragma omp parallel for schedule(static, 1) collapse(2)
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

void SecondOrderComputationKernel::SetComputationParameters(
    ComputationParameters *parameters) {
  this->parameters = (AcousticOmpComputationParameters *)(parameters);
  if (this->parameters == nullptr) {
    std::cout << "Not a compatible computation parameters : "
                 "expected AcousticOmpComputationParameters"
              << std::endl;
    exit(-1);
  }
}

void SecondOrderComputationKernel::SetGridBox(GridBox *grid_box) {
  ;
  this->grid = (AcousticSecondGrid *)(grid_box);
  if (this->grid == nullptr) {
    std::cout << "Not a compatible gridbox : "
                 "expected AcousticSecondGrid"
              << std::endl;
    exit(-1);
  }
}
