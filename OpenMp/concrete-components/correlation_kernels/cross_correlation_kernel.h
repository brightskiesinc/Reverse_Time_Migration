//
// Created by mirnamoawad on 10/30/19.
//
#ifndef ACOUSTIC2ND_RTM_RTM_CORRELATION_KERNEL_H
#define ACOUSTIC2ND_RTM_RTM_CORRELATION_KERNEL_H

#include <concrete-components/data_units/acoustic_openmp_computation_parameters.h>
#include <concrete-components/data_units/acoustic_second_grid.h>
#include <concrete-components/data_units/staggered_grid.h>
#include <cstdlib>
#include <skeleton/components/correlation_kernel.h>

class CrossCorrelationKernel : public CorrelationKernel {
private:
  AcousticOmpComputationParameters *parameters;
  GridBox *grid;
  float *shot_correlation;
  float *total_correlation;
  size_t num_bytes;

public:
  void Stack() override;

  void Correlate(GridBox *in_1) override;

  void ResetShotCorrelation() override;

  float *GetShotCorrelation() override;

  float *GetStackedShotCorrelation() override;

  void SetComputationParameters(ComputationParameters *parameters) override;

  void SetGridBox(GridBox *grid_box) override;

  MigrationData *GetMigrationData() override;

  ~CrossCorrelationKernel() override;

  CrossCorrelationKernel();
};

#endif // ACOUSTIC2ND_RTM_RTM_CORRELATION_KERNEL_H
