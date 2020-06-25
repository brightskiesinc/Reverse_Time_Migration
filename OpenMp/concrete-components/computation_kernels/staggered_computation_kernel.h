//
// Created by mirnamoawad on 1/9/20.
//

#ifndef ACOUSTIC2ND_RTM_STAGGERED_COMPUTATION_KERNEL_H
#define ACOUSTIC2ND_RTM_STAGGERED_COMPUTATION_KERNEL_H

#include <concrete-components/data_units/acoustic_openmp_computation_parameters.h>
#include <concrete-components/data_units/staggered_grid.h>
#include <skeleton/components/computation_kernel.h>

class StaggeredComputationKernel : public ComputationKernel {
private:
  StaggeredGrid *grid;
  AcousticOmpComputationParameters *parameters;
  bool is_forward;

public:
  StaggeredComputationKernel(bool is_forward = true);

  ~StaggeredComputationKernel() override;

  void Step() override;

  void FirstTouch(float *ptr, uint nx, uint nz, uint ny) override;

  void SetComputationParameters(ComputationParameters *parameters) override;

  void SetGridBox(GridBox *grid_box) override;
};

#endif // ACOUSTIC2ND_RTM_STAGGERED_COMPUTATION_KERNEL_H
