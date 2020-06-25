//
// Created by mirnamoawad on 10/28/19.
//

#ifndef ACOUSTIC2ND_RTM_COMPUTATION_KERNEL_RTM_COMPUTATION_KERNEL_H
#define ACOUSTIC2ND_RTM_COMPUTATION_KERNEL_RTM_COMPUTATION_KERNEL_H

#include <concrete-components/data_units/acoustic_openmp_computation_parameters.h>
#include <concrete-components/data_units/acoustic_second_grid.h>
#include <skeleton/components/computation_kernel.h>

class SecondOrderComputationKernel : public ComputationKernel {
private:
  AcousticSecondGrid *grid;
  AcousticOmpComputationParameters *parameters;

public:
  SecondOrderComputationKernel();
  ~SecondOrderComputationKernel() override;

  void Step() override;

  void FirstTouch(float *ptr, uint nx, uint nz, uint ny) override;

  void SetComputationParameters(ComputationParameters *parameters) override;

  void SetGridBox(GridBox *grid_box) override;
};

#endif // ACOUSTIC2ND_RTM_COMPUTATION_KERNEL_RTM_COMPUTATION_KERNEL_H
