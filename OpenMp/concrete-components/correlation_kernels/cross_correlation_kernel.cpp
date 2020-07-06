//
// Created by mirnamoawad on 10/30/19.
//
#include "cross_correlation_kernel.h"
#include <cmath>
#include <cstring>
#include <iostream>
#include <omp.h>
#include <skeleton/helpers/memory_allocation/memory_allocator.h>
#include <skeleton/helpers/timer/timer.hpp>

using namespace std;
#define EPSILON 1e-20

template <bool is_2D>
void Correlation(float *out, GridBox *in_1, GridBox *in_2,
                 AcousticOmpComputationParameters *parameters,
				 float* source_illumination, float* receiver_illumination) {

  GridBox *in_grid_1 = in_1;
  GridBox *in_grid_2 = in_2;
  int nx = in_2->grid_size.nx;
  int ny = in_2->grid_size.ny;
  int nz = in_2->grid_size.nz;
  int wnx = in_2->window_size.window_nx;
  int wny = in_2->window_size.window_ny;
  int wnz = in_2->window_size.window_nz;
  float *frame_1 = in_grid_1->pressure_current;
  float *frame_2 = in_grid_2->pressure_current;
  float *curr_1;
  float *curr_2;
  float *source_i;
  float *receive_i;
  float *output = out;
  float *curr_o;
  uint offset = parameters->half_length;
  int nxEnd = wnx - offset;
  int nyEnd;
  int nzEnd = wnz - offset;
  int y_start;
  if (!is_2D) {
    y_start = offset;
    nyEnd = wny - offset;
  } else {
    y_start = 0;
    nyEnd = 1;
  }
#pragma omp parallel default(shared)
  {
    const uint block_x = parameters->block_x;
    const uint block_y = parameters->block_y;
    const uint block_z = parameters->block_z;

#pragma omp for schedule(static, 1) collapse(2)
    for (int by = y_start; by < nyEnd; by += block_y) {
      for (int bz = offset; bz < nzEnd; bz += block_z) {
        for (int bx = offset; bx < nxEnd; bx += block_x) {

          int izEnd = fmin(bz + block_z, nzEnd);
          int iyEnd = fmin(by + block_y, nyEnd);
          int ixEnd = fmin(bx + block_x, nxEnd);

          for (int iy = by; iy < iyEnd; ++iy) {
            for (int iz = bz; iz < izEnd; ++iz) {
              uint b_offset = iy * wnx * wnz + iz * wnx;
              curr_1 = frame_1 + b_offset;
              curr_2 = frame_2 + b_offset;
              curr_o = output + b_offset;
              source_i = source_illumination + b_offset;
              receive_i = receiver_illumination + b_offset;

#pragma vector aligned
#pragma ivdep
              for (int ix = bx; ix < ixEnd; ++ix) {
                float value;
                value = curr_1[ix] * curr_2[ix];
                source_i[ix] += curr_1[ix] * curr_1[ix];
                receive_i[ix] += curr_2[ix] * curr_2[ix];
                curr_o[ix] += value;
              }
            }
          }
        }
      }
    }
  }
}

void CrossCorrelationKernel ::Correlate(GridBox *in_1) {
  Timer *timer = Timer::getInstance();
  timer->start_timer("CrossCorrelationKernel::Correlate");
  if (grid->grid_size.ny == 1) {
    Correlation<true>(this->shot_correlation, in_1, grid, parameters, source_illumination, receiver_illumination);
  } else {
    Correlation<false>(this->shot_correlation, in_1, grid, parameters, source_illumination, receiver_illumination);
  }
  timer->stop_timer("CrossCorrelationKernel::Correlate");
}

void CrossCorrelationKernel ::Stack() {
  int wnx = grid->window_size.window_nx;
  int wny = grid->window_size.window_ny;
  int wnz = grid->window_size.window_nz;
  int nx = grid->grid_size.nx;
  int ny = grid->grid_size.ny;
  int nz = grid->grid_size.nz;
  float *in = this->shot_correlation;
  float *out = this->total_correlation + grid->window_size.window_start.x + grid->window_size.window_start.z * nx
          + grid->window_size.window_start.y * nx * nz;
  float *input;
  float *output;
  uint block_x = parameters->block_x;
  uint block_z = parameters->block_z;
  uint block_y = parameters->block_y;
  uint offset = parameters->half_length + parameters->boundary_length;
  int nxEnd = wnx - offset;
  int nyEnd;
  int nzEnd = wnz - offset;
  int y_start;
  if (ny > 1) {
    y_start = offset;
    nyEnd = wny - offset;
  } else {
    y_start = 0;
    nyEnd = 1;
  }
#pragma omp parallel for schedule(static, 1) collapse(2)
  for (int by = y_start; by < nyEnd; by += block_y) {
    for (int bz = offset; bz < nzEnd; bz += block_z) {
      for (int bx = offset; bx < nxEnd; bx += block_x) {

        int izEnd = fmin(bz + block_z, nzEnd);
        int iyEnd = fmin(by + block_y, nyEnd);
        int ixEnd = fmin(bx + block_x, nxEnd);

        for (int iy = by; iy < iyEnd; iy++) {
          for (int iz = bz; iz < izEnd; iz++) {
            input = in + iy * wnx * nz + iz * wnx;
            output = out + iy * nx * nz + iz * nx;
#pragma ivdep
#pragma vector aligned
            for (int ix = bx; ix < ixEnd; ix++) {
              output[ix] += input[ix];
            }
          }
        }
      }
    }
  }
}

CrossCorrelationKernel ::~CrossCorrelationKernel() {
  mem_free((void *)shot_correlation);
  mem_free((void *)total_correlation);
}

void CrossCorrelationKernel::SetComputationParameters(
    ComputationParameters *parameters) {
  this->parameters = (AcousticOmpComputationParameters *)(parameters);
}

void CrossCorrelationKernel::SetCompensation(CompensationType c)
{
	compensation_type = c;
}

void CrossCorrelationKernel::SetGridBox(GridBox *grid_box) {
  this->grid = grid_box;
  shot_correlation = (float *)mem_allocate(
      sizeof(float),
      grid_box->window_size.window_nx * grid_box->window_size.window_nz * grid_box->window_size.window_ny,
      "shot_correlation");
  total_correlation = (float *)mem_allocate(
      sizeof(float),
      grid_box->grid_size.nx * grid_box->grid_size.nz * grid_box->grid_size.ny,
      "stacked_shot_correlation");
  num_bytes = grid_box->grid_size.nx * grid_box->grid_size.nz *
              grid_box->grid_size.ny * sizeof(float);
  memset(total_correlation, 0, num_bytes);

  source_illumination = (float *)mem_allocate(
      sizeof(float),
      grid_box->grid_size.nx * grid_box->grid_size.nz * grid_box->grid_size.ny,
      "stacked_source_illumination");
  num_bytes = grid_box->grid_size.nx * grid_box->grid_size.nz *
              grid_box->grid_size.ny * sizeof(float);
  memset(source_illumination, 0, num_bytes);

  receiver_illumination = (float *)mem_allocate(
      sizeof(float),
      grid_box->grid_size.nx * grid_box->grid_size.nz * grid_box->grid_size.ny,
      "stacked_reciever_illumination");
  num_bytes = grid_box->grid_size.nx * grid_box->grid_size.nz *
              grid_box->grid_size.ny * sizeof(float);
  memset(receiver_illumination, 0, num_bytes);
}

CrossCorrelationKernel::CrossCorrelationKernel() {}

void CrossCorrelationKernel::ResetShotCorrelation() {
  uint window_bytes = sizeof(float) * grid->window_size.window_nx *
          grid->window_size.window_nz * grid->window_size.window_ny;
  memset(shot_correlation, 0, window_bytes);
}

float *CrossCorrelationKernel::GetShotCorrelation() {
  return this->shot_correlation;
}

float *CrossCorrelationKernel::GetStackedShotCorrelation() {
  return this->total_correlation;
}

MigrationData *CrossCorrelationKernel::GetMigrationData() {

	float* target;
	switch(compensation_type)
	{
	case NO_COMPENSATION:
		target = this->total_correlation;
		break;
	case SOURCE_COMPENSATION:
		target = GetSourceCompensationCorrelation();
		break;
	case RECEIVER_COMPENSATION:
		target = GetReceiverCompensationCorrelation();
		break;
	case COMBINED_COMPENSATION:
		target = GetCombinedCompensationCorrelation();
		break;
	default:
		target = GetCombinedCompensationCorrelation();
		break;
	}
	return new MigrationData(
      grid->grid_size.nx, grid->grid_size.nz, grid->grid_size.ny, grid->nt,
      grid->cell_dimensions.dx, grid->cell_dimensions.dz,
      grid->cell_dimensions.dy, grid->dt, target);
}

float *CrossCorrelationKernel::GetSourceCompensationCorrelation()
{
	uint size = grid->grid_size.nx * grid->grid_size.nz * grid->grid_size.ny;
	source_illumination_compensation = new float[size];
	for(int i = 0; i < size; i++)
	{
            source_illumination_compensation[i] = total_correlation[i] / (source_illumination[i] + EPSILON);
	}

	return source_illumination_compensation;
}

float *CrossCorrelationKernel::GetReceiverCompensationCorrelation()
{
	uint size = grid->grid_size.nx * grid->grid_size.nz * grid->grid_size.ny;
	receiver_illumination_compensation = new float[size];
	for(int i = 0; i < size; i++)
	{
		    receiver_illumination_compensation[i] = total_correlation[i] / (receiver_illumination[i] + EPSILON);
	}

	return receiver_illumination_compensation;
}

float *CrossCorrelationKernel::GetCombinedCompensationCorrelation()
{
	uint size = grid->grid_size.nx * grid->grid_size.nz * grid->grid_size.ny;
	combined_illumination_compensation = new float[size];
	for(int i = 0; i < size; i++)
	{
		combined_illumination_compensation[i] = total_correlation[i] / sqrt(source_illumination[i] * receiver_illumination[i]  + EPSILON);
	}

	return combined_illumination_compensation;
}
