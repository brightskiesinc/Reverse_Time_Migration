//
// Created by mirnamoawad on 10/28/19.
//

#ifndef ACOUSTIC2ND_RTM_COMPUTATION_KERNEL_RTM_COMPUTATION_KERNEL_H
#define ACOUSTIC2ND_RTM_COMPUTATION_KERNEL_RTM_COMPUTATION_KERNEL_H

#include <CL/sycl.hpp>
#include <concrete-components/data_units/acoustic_dpc_computation_parameters.h>
#include <concrete-components/data_units/acoustic_second_grid.h>
#include <skeleton/components/computation_kernel.h>

using namespace cl::sycl;
using namespace std;

class SecondOrderComputationKernel : public ComputationKernel {
private:
  AcousticSecondGrid *grid;
  AcousticDpcComputationParameters *parameters;
  float *d_coeff_x;
  float *d_coeff_y;
  float *d_coeff_z;
  int *d_front;
  int *d_vertical;
  float coeff_xyz;
  sycl::queue *in_queue;

public:
  SecondOrderComputationKernel();
  ~SecondOrderComputationKernel() override;

  void Step() override;

  void FirstTouch(float *ptr, uint nx, uint nz, uint ny) override;

  void SetComputationParameters(ComputationParameters *parameters) override;

  void SetGridBox(GridBox *grid_box) override;

  template <bool is_2D, HALF_LENGTH half_length>
  void Computation_syclDevice(AcousticSecondGrid *grid,
                              AcousticDpcComputationParameters *parameters);
};

#endif // ACOUSTIC2ND_RTM_COMPUTATION_KERNEL_RTM_COMPUTATION_KERNEL_H
