//
// Created by mirnamoawad on 10/30/19.
//
#include "cross_correlation_kernel.h"
#include <cmath>
#include <iostream>
#include <rtm-framework/skeleton/helpers/timer/timer.hpp>

using namespace cl::sycl;

template <bool is_2D, bool stack>
void Correlation(float *output_buffer, AcousticSecondGrid *in_1,
                 AcousticSecondGrid *in_2,
                 AcousticDpcComputationParameters *parameters) {
  int nx = in_2->grid_size.nx;
  int ny = in_2->grid_size.ny;
  int nz = in_2->grid_size.nz;
  AcousticDpcComputationParameters::device_queue->submit([&](handler &cgh) {
    auto global_range = range<1>(nx * ny * nz);
    auto local_range = range<1>(parameters->cor_block);
    auto global_nd_range = nd_range<1>(global_range, local_range);

    float *pressure_1 = in_1->pressure_current;
    float *pressure_2 = in_2->pressure_current;
    cgh.parallel_for<class cross_correlation>(
        global_nd_range, [=](nd_item<1> it) {
          int idx = it.get_global_linear_id();
          output_buffer[idx] += pressure_1[idx] * pressure_2[idx];
        });
  });
  AcousticDpcComputationParameters::device_queue->wait();
}

CrossCorrelationKernel ::~CrossCorrelationKernel() = default;

void CrossCorrelationKernel::SetComputationParameters(
    ComputationParameters *parameters) {
  this->parameters = (AcousticDpcComputationParameters *)(parameters);
  if (this->parameters == nullptr) {
    std::cout << "Not a compatible computation parameters : "
                 "expected AcousticDpcComputationParameters"
              << std::endl;
    exit(-1);
  }
}

void CrossCorrelationKernel::SetGridBox(GridBox *grid_box) {
  this->grid = (AcousticSecondGrid *)(grid_box);
  internal_queue = AcousticDpcComputationParameters::device_queue;
  if (this->grid == nullptr) {
    std::cout << "Not a compatible gridbox : "
                 "expected AcousticSecondGrid"
              << std::endl;
    exit(-1);
  }
}

void CrossCorrelationKernel::ResetShotCorrelation() {
  int nx = grid->grid_size.nx;
  int ny = grid->grid_size.ny;
  int nz = grid->grid_size.nz;
  uint sizeTotal = nx * ny * nz;
  if (correlation_buffer == nullptr) {
    correlation_buffer = (float *)cl::sycl::malloc_device(
        sizeof(float) * sizeTotal + 16 * sizeof(float),
        internal_queue->get_device(), internal_queue->get_context());
    correlation_buffer = &(correlation_buffer[16 - parameters->half_length]);
  }
  internal_queue->submit([&](handler &cgh) {
    cgh.memset(correlation_buffer, 0, sizeof(float) * sizeTotal);
  });
  internal_queue->wait();
}

void CrossCorrelationKernel::Stack() {
  int nx = grid->grid_size.nx;
  int ny = grid->grid_size.ny;
  int nz = grid->grid_size.nz;

  size_t sizeTotal = nx * nz * ny;
  if (stack_buffer == nullptr) {
    stack_buffer = (float *)cl::sycl::malloc_device(
        sizeof(float) * sizeTotal + 16 * sizeof(float),
        internal_queue->get_device(), internal_queue->get_context());
    stack_buffer = &(stack_buffer[16 - parameters->half_length]);

    internal_queue->submit([&](handler &cgh) {
      cgh.memset(stack_buffer, 0, sizeof(float) * sizeTotal);
    });
    internal_queue->wait();
  }
  internal_queue->submit([&](handler &cgh) {
    auto global_range = range<1>(nx * ny * nz);
    float *stack_buf = stack_buffer;
    float *cor_buf = correlation_buffer;
    cgh.parallel_for<class cross_correlation>(
        global_range, [=](id<1> idx) { stack_buf[idx[0]] += cor_buf[idx[0]]; });
  });
  internal_queue->wait();
}

void CrossCorrelationKernel::Correlate(GridBox *in_1) {
  AcousticSecondGrid *in_grid = (AcousticSecondGrid *)in_1;

  Timer *timer = Timer::getInstance();
  timer->start_timer("CrossCorrelationKernel::Correlate");

  if (grid->grid_size.ny == 1) {
    Correlation<true, true>(correlation_buffer, in_grid, grid, parameters);
  } else {
    Correlation<false, true>(correlation_buffer, in_grid, grid, parameters);
  }

  timer->stop_timer("CrossCorrelationKernel::Correlate");
}

float *CrossCorrelationKernel::GetShotCorrelation() {

  int nx = grid->grid_size.nx;
  int ny = grid->grid_size.ny;
  int nz = grid->grid_size.nz;
  size_t sizeTotal = nx * nz * ny;
  if (temp_correlation_buffer == nullptr) {
      temp_correlation_buffer = new float[sizeTotal];
  }
  internal_queue->wait();
  internal_queue->submit([&](handler &cgh) {
    cgh.memcpy(temp_correlation_buffer, correlation_buffer, sizeTotal * sizeof(float));
  });
  internal_queue->wait();
  return temp_correlation_buffer;
}

float *CrossCorrelationKernel::GetStackedShotCorrelation() {

  int nx = grid->grid_size.nx;
  int ny = grid->grid_size.ny;
  int nz = grid->grid_size.nz;
  size_t sizeTotal = nx * nz * ny;
  if (temp_stack_buffer == nullptr) {
      temp_stack_buffer = new float[sizeTotal];
      memset(temp_stack_buffer, 0, sizeTotal * sizeof(float));
  }
  if (stack_buffer != nullptr) {
    internal_queue->wait();
    internal_queue->submit([&](handler &cgh) {
      cgh.memcpy(temp_stack_buffer, stack_buffer, sizeTotal * sizeof(float));
    });
    internal_queue->wait();
  }
  return temp_stack_buffer;
}

MigrationData *CrossCorrelationKernel::GetMigrationData() {
  float *temp = GetStackedShotCorrelation();;
  uint nz = grid->grid_size.nz;
  uint nx = grid->grid_size.nx;
  uint org_nx = grid->original_dimensions.nx;
  uint org_nz = grid->original_dimensions.nz;
  auto unpadded_temp = new float[org_nx * org_nz];
  for (int i = 0; i < org_nz; i++) {
    for (int j = 0; j < org_nx; j++) {
      unpadded_temp[i * org_nx + j] = temp[i * nx + j];
    }
  }
  return new MigrationData(grid->original_dimensions.nx,
                           grid->original_dimensions.nz,
                           grid->original_dimensions.ny, grid->nt,
                           grid->cell_dimensions.dx, grid->cell_dimensions.dz,
                           grid->cell_dimensions.dy, grid->dt, unpadded_temp);
}