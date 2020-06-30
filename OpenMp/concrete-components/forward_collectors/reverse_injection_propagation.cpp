//
// Created by amr on 19/12/2019.
//

#include "reverse_injection_propagation.h"
#include <iostream>

ReverseInjectionPropagation::ReverseInjectionPropagation(
    ComputationKernel *kernel) {
  this->internal_grid = (AcousticSecondGrid *)mem_allocate(
      sizeof(AcousticSecondGrid), 1, "forward_collector_gridbox");
  this->internal_grid->pressure_current = nullptr;
  this->computation_kernel = kernel;
  this->computation_kernel->SetGridBox(internal_grid);
  this->size_of_boundaries = 0;
  this->time_step = 0;
  this->backup_boundaries = nullptr;
}

void ReverseInjectionPropagation::FetchForward(void) {
  this->computation_kernel->Step();
  time_step--;
  RestoreBoundaries(main_grid, internal_grid, parameters, backup_boundaries,
                    time_step, size_of_boundaries);
}

void ReverseInjectionPropagation::ResetGrid(bool forward_run) {
  unsigned int const grid_size = main_grid->window_size.window_nx *
                                 main_grid->window_size.window_ny *
                                 main_grid->window_size.window_nz;
  float *temp;
  uint nx = main_grid->window_size.window_nx;
  uint nz = main_grid->window_size.window_nz;
  uint ny = main_grid->window_size.window_ny;
  if (!forward_run) {
    if (internal_grid->pressure_current == NULL) {
      internal_grid->pressure_previous = (float *)mem_allocate(
          sizeof(float), grid_size, "forward_collector_pressure_prev",
          parameters->half_length, 16);
      internal_grid->pressure_current = (float *)mem_allocate(
          sizeof(float), grid_size, "forward_collector_pressure_curr",
          parameters->half_length, 32);
      internal_grid->pressure_next = internal_grid->pressure_previous;
      this->computation_kernel->FirstTouch(internal_grid->pressure_previous, nx,
                                           nz, ny);
      this->computation_kernel->FirstTouch(internal_grid->pressure_current, nx,
                                           nz, ny);
      this->computation_kernel->FirstTouch(internal_grid->pressure_next, nx, nz,
                                           ny);
    }
    memcpy(internal_grid->pressure_previous, main_grid->pressure_previous,
           grid_size * sizeof(float));
    memcpy(internal_grid->pressure_current, main_grid->pressure_current,
           grid_size * sizeof(float));
    memcpy(internal_grid->pressure_next, main_grid->pressure_next,
           grid_size * sizeof(float));
    internal_grid->nt = main_grid->nt;
    internal_grid->dt = main_grid->dt;
    memcpy(&internal_grid->grid_size, &main_grid->grid_size,
           sizeof(main_grid->grid_size));
    memcpy(&internal_grid->window_size, &main_grid->window_size,
           sizeof(main_grid->window_size));
    memcpy(&internal_grid->cell_dimensions, &main_grid->cell_dimensions,
           sizeof(main_grid->cell_dimensions));
    internal_grid->velocity = main_grid->velocity;
    internal_grid->window_velocity = main_grid->window_velocity;
    // Swap previous and current to reverse time.
    temp = internal_grid->pressure_previous;
    internal_grid->pressure_previous = internal_grid->pressure_current;
    internal_grid->pressure_current = temp;
    // Only use two pointers, prev is same as next.
    internal_grid->pressure_next = internal_grid->pressure_previous;
  } else {
    if (this->backup_boundaries != nullptr) {
      mem_free(this->backup_boundaries);
    }
    time_step = 0;
    uint half_length = this->parameters->half_length;
    uint bound_length = this->parameters->boundary_length;
    uint nxi =
        main_grid->window_size.window_nx - 2 * (half_length + bound_length);
    uint nzi =
        main_grid->window_size.window_nz - 2 * (half_length + bound_length);
    uint nyi = main_grid->window_size.window_ny;
    if (nyi != 1) {
      nyi = nyi - 2 * (half_length + bound_length);
    }
    this->size_of_boundaries =
        nxi * nyi * half_length * 2 + nzi * nyi * half_length * 2;
    if (nyi != 1) {
      this->size_of_boundaries += nxi * nzi * half_length * 2;
    }
    this->backup_boundaries = (float *)mem_allocate(
        sizeof(float), this->size_of_boundaries * (main_grid->nt + 1),
        "boundary memory");
  }
  memset(main_grid->pressure_previous, 0.0f, grid_size * sizeof(float));
  memset(main_grid->pressure_current, 0.0f, grid_size * sizeof(float));
  memset(main_grid->pressure_next, 0.0f, grid_size * sizeof(float));
}

void ReverseInjectionPropagation::SaveForward() {
  SaveBoundaries(main_grid, parameters, backup_boundaries, time_step,
                 size_of_boundaries);
  time_step++;
}

ReverseInjectionPropagation::~ReverseInjectionPropagation() {
  if (internal_grid->pressure_current != NULL) {
    mem_free((void *)internal_grid->pressure_previous);
    mem_free((void *)internal_grid->pressure_current);
  }
  mem_free((void *)internal_grid);
  delete computation_kernel;
}

void ReverseInjectionPropagation::SetComputationParameters(
    ComputationParameters *parameters) {
  this->parameters = parameters;
  this->computation_kernel->SetComputationParameters(parameters);
}

void ReverseInjectionPropagation::SetGridBox(GridBox *grid_box) {
  this->main_grid = (AcousticSecondGrid *)(grid_box);
  if (this->main_grid == nullptr) {
    std::cout << "Not a compatible gridbox : "
                 "expected AcousticSecondGrid"
              << std::endl;
    exit(-1);
  }
}

GridBox *ReverseInjectionPropagation::GetForwardGrid() { return internal_grid; }
