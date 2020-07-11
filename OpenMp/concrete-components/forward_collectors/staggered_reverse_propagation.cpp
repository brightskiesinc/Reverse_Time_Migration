//
// Created by mirnamoawad on 1/20/20.
//

#include "staggered_reverse_propagation.h"
#include <iostream>

StaggeredReversePropagation::StaggeredReversePropagation(
    ComputationKernel *kernel) {
  this->internal_grid = (StaggeredGrid *)mem_allocate(
      sizeof(StaggeredGrid), 1, "forward_collector_gridbox");
  this->internal_grid = (StaggeredGrid *)this->internal_grid;
  this->internal_grid->pressure_current = nullptr;
  this->computation_kernel = kernel;
  this->computation_kernel->SetGridBox(internal_grid);
}

void StaggeredReversePropagation::FetchForward(
    void) { // staggeredcomputationkernel
  this->computation_kernel->Step();
}

void StaggeredReversePropagation::ResetGrid(bool forward_run) {
  unsigned int const grid_size = main_grid->window_size.window_nx *
                                 main_grid->window_size.window_ny *
                                 main_grid->window_size.window_nz;
  float *temp;
  uint nx = main_grid->window_size.window_nx;
  uint nz = main_grid->window_size.window_nz;
  uint ny = main_grid->window_size.window_ny;
  if (!forward_run) {
    if (internal_grid->pressure_current == NULL) {
      internal_grid->pressure_current = (float *)mem_allocate(
          sizeof(float), grid_size, "forward_collector_pressure_curr",
          parameters->half_length, 16);
      internal_grid->pressure_next = internal_grid->pressure_current;
      internal_grid->particle_velocity_x_current =
          (float *)mem_allocate(sizeof(float), grid_size,
                                "forward_collector_particle_velocity_x_current",
                                parameters->half_length, 32);
      internal_grid->particle_velocity_z_current =
          (float *)mem_allocate(sizeof(float), grid_size,
                                "forward_collector_particle_velocity_z_current",
                                parameters->half_length, 48);

      if (ny > 1) {
        internal_grid->particle_velocity_y_current = (float *)mem_allocate(
            sizeof(float), grid_size,
            "forward_collector_particle_velocity_y_current",
            parameters->half_length, 64);
      }
      this->computation_kernel->FirstTouch(internal_grid->pressure_current, nx,
                                           nz, ny);
      this->computation_kernel->FirstTouch(internal_grid->pressure_next, nx, nz,
                                           ny);
      this->computation_kernel->FirstTouch(
          internal_grid->particle_velocity_x_current, nx, nz, ny);
      this->computation_kernel->FirstTouch(
          internal_grid->particle_velocity_z_current, nx, nz, ny);
      if (ny > 1) {
        this->computation_kernel->FirstTouch(
            internal_grid->particle_velocity_y_current, nx, nz, ny);
      }
    }
    memcpy(internal_grid->pressure_current, main_grid->pressure_current,
           grid_size * sizeof(float));
    memcpy(internal_grid->pressure_next, main_grid->pressure_next,
           grid_size * sizeof(float));
    memcpy(internal_grid->particle_velocity_x_current,
           main_grid->particle_velocity_x_current, grid_size * sizeof(float));
    memcpy(internal_grid->particle_velocity_z_current,
           main_grid->particle_velocity_z_current, grid_size * sizeof(float));
    if (ny > 1) {
      memcpy(internal_grid->particle_velocity_y_current,
             main_grid->particle_velocity_y_current, grid_size * sizeof(float));
    }
    internal_grid->nt = main_grid->nt;
    internal_grid->dt = main_grid->dt;
    memcpy(&internal_grid->grid_size, &main_grid->grid_size,
           sizeof(main_grid->grid_size));
    memcpy(&internal_grid->window_size, &main_grid->window_size,
           sizeof(main_grid->window_size));
    memcpy(&internal_grid->cell_dimensions, &main_grid->cell_dimensions,
           sizeof(main_grid->cell_dimensions));
    internal_grid->velocity = main_grid->velocity;
    internal_grid->density = main_grid->density;
    internal_grid->window_velocity = main_grid->window_velocity;
    internal_grid->window_density = main_grid->window_density;
    // Swap previous and current to reverse time.
    temp = internal_grid->pressure_current;
    internal_grid->pressure_next = internal_grid->pressure_current;
    internal_grid->pressure_current = temp;
  }
  memset(main_grid->pressure_current, 0.0f, grid_size * sizeof(float));
  memset(main_grid->pressure_next, 0.0f, grid_size * sizeof(float));
  memset(main_grid->particle_velocity_x_current, 0.0f,
         grid_size * sizeof(float));
  memset(main_grid->particle_velocity_z_current, 0.0f,
         grid_size * sizeof(float));
  if (ny > 1) {
    memset(main_grid->particle_velocity_y_current, 0.0f,
           grid_size * sizeof(float));
  }
}

void StaggeredReversePropagation::SaveForward() {}

StaggeredReversePropagation::~StaggeredReversePropagation() {
  if (internal_grid->pressure_current != NULL) {
    mem_free((void *)internal_grid->pressure_current);
  }
  mem_free((void *)internal_grid);
  delete computation_kernel;
}

void StaggeredReversePropagation::SetComputationParameters(
    ComputationParameters *parameters) {
  this->parameters = parameters;
  this->computation_kernel->SetComputationParameters(parameters);
}

void StaggeredReversePropagation::SetGridBox(GridBox *grid_box) {
  this->main_grid = (StaggeredGrid *)(grid_box);
  if (this->main_grid == nullptr) {
    std::cout << "Not a compatible gridbox : "
                 "expected StaggeredGridBox"
              << std::endl;
    exit(-1);
  }
}

GridBox *StaggeredReversePropagation::GetForwardGrid() { return internal_grid; }
