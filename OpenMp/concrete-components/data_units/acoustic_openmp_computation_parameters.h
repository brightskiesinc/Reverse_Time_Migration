//
// Created by amr on 08/12/2019.
//

#ifndef ACOUSTIC2ND_RTM_ACOUSTIC_OPENMP_COMPUTATION_PARAMETERS_H
#define ACOUSTIC2ND_RTM_ACOUSTIC_OPENMP_COMPUTATION_PARAMETERS_H

#include <skeleton/base/datatypes.h>

class AcousticOmpComputationParameters : public ComputationParameters {
public:
  uint block_x;
  uint block_y;
  uint block_z;
  uint n_threads;
  explicit AcousticOmpComputationParameters(HALF_LENGTH half_length)
      : ComputationParameters(half_length) {
    block_x = 512;
    block_y = 15;
    block_z = 44;
    n_threads = 16;
  }
};

#endif // ACOUSTIC2ND_RTM_ACOUSTIC_OPENMP_COMPUTATION_PARAMETERS_H
