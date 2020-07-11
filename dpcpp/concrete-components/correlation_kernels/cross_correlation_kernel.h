//
// Created by mirnamoawad on 10/30/19.
//
#ifndef ACOUSTIC2ND_RTM_RTM_CORRELATION_KERNEL_H
#define ACOUSTIC2ND_RTM_RTM_CORRELATION_KERNEL_H

#include <concrete-components/data_units/acoustic_dpc_computation_parameters.h>
#include <concrete-components/data_units/acoustic_second_grid.h>
#include <skeleton/components/correlation_kernel.h>

class CrossCorrelationKernel : public CorrelationKernel {
private:
  AcousticDpcComputationParameters *parameters;
  AcousticSecondGrid *grid;
  sycl::queue *internal_queue;
  float *correlation_buffer = nullptr;
  float *stack_buffer = nullptr;
  float *temp_correlation_buffer = nullptr;
  float *temp_stack_buffer = nullptr;
  float *stack_source_illumination;
  float *source_illumination;
  float *stack_receiver_illumination;
  float *receiver_illumination;

  float *source_illumination_compensation;
  float *receiver_illumination_compensation;
  float *combined_illumination_compensation;

  CompensationType compensation_type;

public:
  void ResetShotCorrelation() override;

  void Stack() override;

  void Correlate(GridBox *in_1) override;

  float *GetShotCorrelation() override;

  float *GetStackedShotCorrelation() override;
  void SetComputationParameters(ComputationParameters *parameters) override;
  void SetGridBox(GridBox *grid_box) override;

  void SetCompensation(CompensationType c) override;

  float *GetSourceCompensationCorrelation() override;

  float *GetReceiverCompensationCorrelation() override;

  float *GetCombinedCompensationCorrelation() override;

  MigrationData *GetMigrationData() override;
  ~CrossCorrelationKernel() override;
};

#endif // ACOUSTIC2ND_RTM_RTM_CORRELATION_KERNEL_H
