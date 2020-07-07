//
// Created by mirnamoawad on 10/30/19.
//
#include "cross_correlation_kernel.h"
#include <cmath>
#include <iostream>

using namespace cl::sycl;
#define EPSILON 1e-20

template <bool is_2D, bool stack>
void Correlation(float *output_buffer, AcousticSecondGrid *in_1,
		AcousticSecondGrid *in_2,
		AcousticDpcComputationParameters *parameters,
		float* source_illumination, float* receiver_illumination) {
	int nx = in_2->window_size.window_nx;
	int ny = in_2->window_size.window_ny;
	int nz = in_2->window_size.window_nz;
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
			source_illumination[idx] += pressure_1[idx] * pressure_1[idx];
			receiver_illumination[idx] += pressure_2[idx] * pressure_2[idx];
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

void CrossCorrelationKernel::SetCompensation(CompensationType c)
{
	compensation_type = c;
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
  int nx = grid->window_size.window_nx;
  int ny = grid->window_size.window_ny;
  int nz = grid->window_size.window_nz;
  uint sizeTotal = nx * ny * nz;
  if (correlation_buffer == nullptr) {
    correlation_buffer = (float *)cl::sycl::malloc_device(
        sizeof(float) * sizeTotal + 16 * sizeof(float),
        internal_queue->get_device(), internal_queue->get_context());
    correlation_buffer = &(correlation_buffer[16 - parameters->half_length]);
    source_illumination = (float *)cl::sycl::malloc_device(
            sizeof(float) * sizeTotal + 16 * sizeof(float),
            internal_queue->get_device(), internal_queue->get_context());
    source_illumination = &(source_illumination[16 - parameters->half_length]);
    receiver_illumination = (float *)cl::sycl::malloc_device(
            sizeof(float) * sizeTotal + 16 * sizeof(float),
            internal_queue->get_device(), internal_queue->get_context());
    receiver_illumination = &(receiver_illumination[16 - parameters->half_length]);
  }
  internal_queue->submit([&](handler &cgh) {
    cgh.memset(correlation_buffer, 0, sizeof(float) * sizeTotal);
  });
  internal_queue->submit([&](handler &cgh) {
    cgh.memset(source_illumination, 0, sizeof(float) * sizeTotal);
  });
  internal_queue->submit([&](handler &cgh) {
    cgh.memset(receiver_illumination, 0, sizeof(float) * sizeTotal);
  });
  internal_queue->wait();
}

void CrossCorrelationKernel::Stack() {
  int nx = grid->grid_size.nx;
  int ny = grid->grid_size.ny;
  int nz = grid->grid_size.nz;
  int wnx = grid->window_size.window_nx;
  int wny = grid->window_size.window_ny;
  int wnz = grid->window_size.window_nz;
  int ele_x = grid->original_dimensions.nx;
  int ele_y = grid->original_dimensions.ny;
  int ele_z = grid->original_dimensions.nz;

  size_t sizeTotal = nx * nz * ny;
  if (stack_buffer == nullptr) {
    stack_buffer = (float *)cl::sycl::malloc_device(
        sizeof(float) * sizeTotal + 16 * sizeof(float),
        internal_queue->get_device(), internal_queue->get_context());
    stack_buffer = &(stack_buffer[16 - parameters->half_length]);
    stack_source_illumination = (float *)cl::sycl::malloc_device(
        sizeof(float) * sizeTotal + 16 * sizeof(float),
        internal_queue->get_device(), internal_queue->get_context());
    stack_source_illumination = &(stack_source_illumination[16 - parameters->half_length]);
    stack_receiver_illumination = (float *)cl::sycl::malloc_device(
        sizeof(float) * sizeTotal + 16 * sizeof(float),
        internal_queue->get_device(), internal_queue->get_context());
    stack_receiver_illumination = &(stack_receiver_illumination[16 - parameters->half_length]);

    internal_queue->submit([&](handler &cgh) {
      cgh.memset(stack_buffer, 0, sizeof(float) * sizeTotal);
    });
    internal_queue->submit([&](handler &cgh) {
      cgh.memset(stack_source_illumination, 0, sizeof(float) * sizeTotal);
    });
    internal_queue->submit([&](handler &cgh) {
      cgh.memset(stack_receiver_illumination, 0, sizeof(float) * sizeTotal);
    });
    internal_queue->wait();
  }
  internal_queue->submit([&](handler &cgh) {
    auto global_range = range<3>(ele_x, ele_z, ele_y);
    float *stack_buf = stack_buffer + grid->window_size.window_start.x + grid->window_size.window_start.z * nx
                       + grid->window_size.window_start.y * nx * nz;
    float *cor_buf = correlation_buffer;
    float *stack_src = stack_source_illumination + grid->window_size.window_start.x + grid->window_size.window_start.z * nx
                         + grid->window_size.window_start.y * nx * nz;
    float *cor_src = source_illumination;
    float *stack_rcv = stack_receiver_illumination + grid->window_size.window_start.x + grid->window_size.window_start.z * nx
                         + grid->window_size.window_start.y * nx * nz;
    float *cor_rcv = receiver_illumination;
    cgh.parallel_for<class cross_correlation>(
        global_range, [=](id<3> idx) {
            uint offset_window = idx[0] + idx[1] * wnx + idx[2] * wnx * wnz;
            uint offset = idx[0] + idx[1] * nx + idx[2] * nx * nz;
            stack_buf[offset] += cor_buf[offset_window];
            stack_src[offset] += cor_src[offset_window];
            stack_rcv[offset] += cor_rcv[offset_window];
        });
  });
  internal_queue->wait();
}

void CrossCorrelationKernel::Correlate(GridBox *in_1) {
  AcousticSecondGrid *in_grid = (AcousticSecondGrid *)in_1;



  if (grid->grid_size.ny == 1) {
    Correlation<true, true>(correlation_buffer, in_grid, grid, parameters, source_illumination, receiver_illumination);
  } else {
    Correlation<false, true>(correlation_buffer, in_grid, grid, parameters, source_illumination, receiver_illumination);
  }

}

float *CrossCorrelationKernel::GetShotCorrelation() {

  int nx = grid->window_size.window_nx;
  int ny = grid->window_size.window_ny;
  int nz = grid->window_size.window_nz;
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

	uint nz = grid->grid_size.nz;
	uint nx = grid->grid_size.nx;
	uint org_nx = grid->full_original_dimensions.nx;
	uint org_nz = grid->full_original_dimensions.nz;

	float *temp;
	float *unpadded_temp;
	float *target;

	switch(compensation_type)
	{
	case NO_COMPENSATION:
		temp = GetStackedShotCorrelation();
		unpadded_temp = new float[org_nx * org_nz];
		for (int i = 0; i < org_nz; i++) {
			for (int j = 0; j < org_nx; j++) {
				unpadded_temp[i * org_nx + j] = temp[i * nx + j];
			}
		}
  		target = unpadded_temp;
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

	return new MigrationData(grid->full_original_dimensions.nx,
			grid->full_original_dimensions.nz,
			grid->full_original_dimensions.ny, grid->nt,
			grid->cell_dimensions.dx, grid->cell_dimensions.dz,
			grid->cell_dimensions.dy, grid->dt, target);
}


float *CrossCorrelationKernel::GetSourceCompensationCorrelation()
{
	uint nz = grid->grid_size.nz;
	uint nx = grid->grid_size.nx;
	uint ny = grid->grid_size.ny;
	uint org_nx = grid->full_original_dimensions.nx;
	uint org_nz = grid->full_original_dimensions.nz;
	uint size = nx * nz * ny;
	// getting host buffers
	float* temp = new float[size];
	float* temp_source = new float[size];
	source_illumination_compensation = new float[org_nx * org_nz];

    internal_queue->submit([&](handler &cgh) {
      cgh.memcpy(temp, stack_buffer, size * sizeof(float));
    });
    internal_queue->wait();

    internal_queue->submit([&](handler &cgh) {
      cgh.memcpy(temp_source, stack_source_illumination, size * sizeof(float));
    });
    internal_queue->wait();

	// unpadding the imagestack_buffer and calculating the compensation
	for (int i = 0; i < org_nz; i++) {
		for (int j = 0; j < org_nx; j++) {
			source_illumination_compensation[i * org_nx + j] = temp[i * nx + j]
				/(temp_source[i * nx + j] + EPSILON);
		}
	}
	delete []temp;
	delete []temp_source;

	return source_illumination_compensation;
}

float *CrossCorrelationKernel::GetReceiverCompensationCorrelation()
{
	uint nz = grid->grid_size.nz;
	uint nx = grid->grid_size.nx;
	uint ny = grid->grid_size.ny;
	uint org_nx = grid->full_original_dimensions.nx;
	uint org_nz = grid->full_original_dimensions.nz;
	uint size = nx * nz * ny;
	// getting host buffers
	float* temp = new float[size];
	float* temp_receiver = new float[size];
	receiver_illumination_compensation = new float[org_nx * org_nz];

    internal_queue->submit([&](handler &cgh) {
      cgh.memcpy(temp, stack_buffer, size * sizeof(float));
    });
    internal_queue->wait();

    internal_queue->submit([&](handler &cgh) {
      cgh.memcpy(temp_receiver, stack_receiver_illumination, size * sizeof(float));
    });
    internal_queue->wait();

	// unpadding the imagestack_buffer and calculating the compensation
	for (int i = 0; i < org_nz; i++) {
		for (int j = 0; j < org_nx; j++) {
			receiver_illumination_compensation[i * org_nx + j] = temp[i * nx + j]
				/(temp_receiver[i * nx + j] + EPSILON);
		}
	}
	delete []temp;
	delete []temp_receiver;

	return receiver_illumination_compensation;
}

float *CrossCorrelationKernel::GetCombinedCompensationCorrelation()
{
	uint nz = grid->grid_size.nz;
	uint nx = grid->grid_size.nx;
	uint ny = grid->grid_size.ny;
	uint org_nx = grid->full_original_dimensions.nx;
	uint org_nz = grid->full_original_dimensions.nz;
	uint size = nx * nz * ny;
	// getting host buffers
	float* temp = new float[size];
	float* temp_source = new float[size];
	float* temp_receiver = new float[size];
	combined_illumination_compensation = new float[org_nx * org_nz];

    internal_queue->submit([&](handler &cgh) {
      cgh.memcpy(temp, stack_buffer, size * sizeof(float));
    });
    internal_queue->wait();

    internal_queue->submit([&](handler &cgh) {
      cgh.memcpy(temp_source, stack_source_illumination, size * sizeof(float));
    });
    internal_queue->wait();

    internal_queue->submit([&](handler &cgh) {
      cgh.memcpy(temp_receiver, stack_receiver_illumination, size * sizeof(float));
    });
    internal_queue->wait();

	// unpadding the imagestack_buffer and calculating the compensation
	for (int i = 0; i < org_nz; i++) {
		for (int j = 0; j < org_nx; j++) {
			combined_illumination_compensation[i * org_nx + j] = temp[i * nx + j]/
					(sqrt(temp_source[i * nx + j] * temp_receiver[i * nx + j]  + EPSILON));
		}
	}
	delete []temp;
	delete []temp_source;
	delete []temp_receiver;

	return combined_illumination_compensation;
}
