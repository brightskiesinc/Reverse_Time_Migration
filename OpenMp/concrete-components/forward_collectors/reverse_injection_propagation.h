//
// Created by amr on 19/12/2019.
//

#ifndef ACOUSTIC2ND_RTM_REVERSE_INJECTION_PROPAGATION_H
#define ACOUSTIC2ND_RTM_REVERSE_INJECTION_PROPAGATION_H
#include <concrete-components/data_units/acoustic_second_grid.h>
#include <concrete-components/forward_collectors/boundary_saver/boundary_saver.h>
#include <cstdlib>
#include <cstring>
#include <skeleton/components/computation_kernel.h>
#include <skeleton/components/forward_collector.h>
#include <skeleton/helpers/memory_allocation/memory_allocator.h>

class ReverseInjectionPropagation : public ForwardCollector {
private:
  AcousticSecondGrid *main_grid;
  AcousticSecondGrid *internal_grid;
  ComputationParameters *parameters;
  ComputationKernel *computation_kernel;
  float *backup_boundaries;
  uint time_step;
  uint size_of_boundaries;

public:
  ReverseInjectionPropagation(ComputationKernel *kernel);
  void FetchForward(void) override;
  void SaveForward() override;
  void ResetGrid(bool forward_run) override;
  void SetComputationParameters(ComputationParameters *parameters) override;
  void SetGridBox(GridBox *grid_box) override;
  GridBox *GetForwardGrid() override;
  ~ReverseInjectionPropagation() override;
};
#endif // ACOUSTIC2ND_RTM_REVERSE_INJECTION_PROPAGATION_H
