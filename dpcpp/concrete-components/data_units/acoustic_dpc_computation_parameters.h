//
// Created by amr on 08/12/2019.
//

#ifndef ACOUSTIC2ND_RTM_ACOUSTIC_DPC_COMPUTATION_PARAMETERS_H
#define ACOUSTIC2ND_RTM_ACOUSTIC_DPC_COMPUTATION_PARAMETERS_H

#include <CL/sycl.hpp>
#include <skeleton/base/datatypes.h>

enum SYCL_DEVICE { CPU, GPU, GPU_SHARED, GPU_SEMI_SHARED };

class AcousticDpcComputationParameters : public ComputationParameters {
public:
  uint block_x;
  uint block_y;
  uint block_z;
  uint cor_block;
  uint n_threads;
  SYCL_DEVICE device;
  static cl::sycl::queue *device_queue;

  explicit AcousticDpcComputationParameters(HALF_LENGTH half_length)
      : ComputationParameters(half_length) {
    block_x = 32;
    block_y = 256;
    block_z = 1;
    cor_block = 64;
    n_threads = 16;
    device = CPU;
    device_queue = nullptr;
  }
};

#endif // ACOUSTIC2ND_RTM_ACOUSTIC_DPC_COMPUTATION_PARAMETERS_H
