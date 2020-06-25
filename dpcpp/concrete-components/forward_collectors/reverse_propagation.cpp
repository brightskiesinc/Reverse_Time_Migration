#include "reverse_propagation.h"
#include <concrete-callbacks/csv_writer.h>
#include <concrete-components/data_units/acoustic_dpc_computation_parameters.h>
#include <iostream>

using namespace cl::sycl;

ReversePropagation::ReversePropagation(ComputationKernel *kernel) {
  this->internal_grid = (AcousticSecondGrid *)mem_allocate(
      sizeof(AcousticSecondGrid), 1, "forward_collector_gridbox");
  this->internal_grid->pressure_current = nullptr;
  this->computation_kernel = kernel;
  parameters = nullptr;
  main_grid = nullptr;
}

void ReversePropagation::FetchForward(void) {
  this->computation_kernel->Step();
}

void ReversePropagation::ResetGrid(bool forward_run) {
  unsigned int const grid_size = main_grid->window_size.window_nx *
                                 main_grid->window_size.window_ny *
                                 main_grid->window_size.window_nz;
  float *temp;
  uint nx = main_grid->window_size.window_nx;
  uint nz = main_grid->window_size.window_nz;
  uint ny = main_grid->window_size.window_ny;
  if (!forward_run) {
    if (internal_grid->pressure_current == NULL) {
      internal_grid->pressure_previous = (float *)cl::sycl::malloc_device(
          sizeof(float) * grid_size + 16 * sizeof(float),
          AcousticDpcComputationParameters::device_queue->get_device(),
          AcousticDpcComputationParameters::device_queue->get_context());
      internal_grid->pressure_previous =
          &(internal_grid->pressure_previous[16 - parameters->half_length]);
      this->computation_kernel->FirstTouch(internal_grid->pressure_previous, nx,
                                           nz, ny);
      internal_grid->pressure_current = (float *)cl::sycl::malloc_device(
          sizeof(float) * grid_size + 16 * sizeof(float),
          AcousticDpcComputationParameters::device_queue->get_device(),
          AcousticDpcComputationParameters::device_queue->get_context());
      internal_grid->pressure_current =
          &(internal_grid->pressure_current[16 - parameters->half_length]);
      this->computation_kernel->FirstTouch(internal_grid->pressure_current, nx,
                                           nz, ny);
      internal_grid->pressure_next = internal_grid->pressure_previous;
    }
    {
      AcousticDpcComputationParameters::device_queue->wait();
      AcousticDpcComputationParameters::device_queue->submit([&](handler &cgh) {
        cgh.memcpy(internal_grid->pressure_previous,
                   main_grid->pressure_previous, grid_size * sizeof(float));
      });
      AcousticDpcComputationParameters::device_queue->submit([&](handler &cgh) {
        cgh.memcpy(internal_grid->pressure_current, main_grid->pressure_current,
                   grid_size * sizeof(float));
      });
      AcousticDpcComputationParameters::device_queue->submit([&](handler &cgh) {
        cgh.memcpy(internal_grid->pressure_next, main_grid->pressure_next,
                   grid_size * sizeof(float));
      });
      AcousticDpcComputationParameters::device_queue->wait();
    }
    internal_grid->nt = main_grid->nt;
    internal_grid->dt = main_grid->dt;
    memcpy(&internal_grid->grid_size, &main_grid->grid_size,
           sizeof(main_grid->grid_size));
    memcpy(&internal_grid->window_size, &main_grid->window_size,
           sizeof(main_grid->window_size));
    memcpy(&internal_grid->cell_dimensions, &main_grid->cell_dimensions,
           sizeof(main_grid->cell_dimensions));
    memcpy(&internal_grid->original_dimensions, &main_grid->original_dimensions,
           sizeof(main_grid->original_dimensions));
    internal_grid->compute_nx = main_grid->compute_nx;
    internal_grid->velocity = main_grid->velocity;
    // Swap previous and current to reverse time.
    temp = internal_grid->pressure_previous;
    internal_grid->pressure_previous = internal_grid->pressure_current;
    internal_grid->pressure_current = temp;
    // Only use two pointers, prev is same as next.
    internal_grid->pressure_next = internal_grid->pressure_previous;
  }
  {
    AcousticDpcComputationParameters::device_queue->submit([&](handler &cgh) {
      cgh.memset(main_grid->pressure_previous, 0, sizeof(float) * grid_size);
    });
    AcousticDpcComputationParameters::device_queue->submit([&](handler &cgh) {
      cgh.memset(main_grid->pressure_current, 0, sizeof(float) * grid_size);
    });
    AcousticDpcComputationParameters::device_queue->submit([&](handler &cgh) {
      cgh.memset(main_grid->pressure_next, 0, sizeof(float) * grid_size);
    });
  }
}

void ReversePropagation::SaveForward() {}

ReversePropagation::~ReversePropagation() {
  if (internal_grid->pressure_current != NULL) {
    cl::sycl::free(
        &(internal_grid->pressure_previous[-(16 - parameters->half_length)]),
        internal_queue->get_context());
    cl::sycl::free(
        &(internal_grid->pressure_current[-(16 - parameters->half_length)]),
        internal_queue->get_context());
  }
  mem_free((void *)internal_grid);
  delete computation_kernel;
}

void ReversePropagation::SetComputationParameters(
    ComputationParameters *parameters) {
  this->parameters = parameters;
  this->computation_kernel->SetComputationParameters(parameters);
}

void ReversePropagation::SetGridBox(GridBox *grid_box) {
  this->main_grid = (AcousticSecondGrid *)(grid_box);
  internal_queue = AcousticDpcComputationParameters::device_queue;
  if (this->main_grid == nullptr) {
    std::cout << "Not a compatible gridbox : "
                 "expected AcousticSecondGrid"
              << std::endl;
    exit(-1);
  }
  internal_grid->dt = main_grid->dt;
  memcpy(&internal_grid->grid_size, &main_grid->grid_size,
         sizeof(main_grid->grid_size));
  memcpy(&internal_grid->window_size, &main_grid->window_size,
         sizeof(main_grid->window_size));
  memcpy(&internal_grid->cell_dimensions, &main_grid->cell_dimensions,
         sizeof(main_grid->cell_dimensions));
  memcpy(&internal_grid->original_dimensions, &main_grid->original_dimensions,
         sizeof(main_grid->original_dimensions));
  internal_grid->compute_nx = main_grid->compute_nx;
  this->computation_kernel->SetGridBox(internal_grid);
}

GridBox *ReversePropagation::GetForwardGrid() { return internal_grid; }
