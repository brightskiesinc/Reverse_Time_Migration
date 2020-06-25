//
// Created by mirnamoawad on 1/20/20.
//

#ifndef ACOUSTIC2ND_RTM_STAGGERED_REVERSE_INJECTION_PROPAGATION_H
#define ACOUSTIC2ND_RTM_STAGGERED_REVERSE_INJECTION_PROPAGATION_H
#include <concrete-components/data_units/staggered_grid.h>
#include <concrete-components/forward_collectors/boundary_saver/boundary_saver.h>
#include <cstdlib>
#include <cstring>
#include <skeleton/components/computation_kernel.h>
#include <skeleton/components/forward_collector.h>
#include <skeleton/helpers/memory_allocation/memory_allocator.h>

class StaggeredReverseInjectionPropagation : public ForwardCollector {
private:
  StaggeredGrid *main_grid;
  StaggeredGrid *internal_grid;
  ComputationParameters *parameters;
  ComputationKernel *computation_kernel;
  float *backup_boundaries;
  uint time_step;
  uint size_of_boundaries;

public:
  StaggeredReverseInjectionPropagation(ComputationKernel *kernel);
  void FetchForward(void) override;
  void SaveForward() override;
  void ResetGrid(bool forward_run) override;
  void SetComputationParameters(ComputationParameters *parameters) override;
  void SetGridBox(GridBox *grid_box) override;
  GridBox *GetForwardGrid() override;
  ~StaggeredReverseInjectionPropagation() override;
};

#endif // ACOUSTIC2ND_RTM_STAGGERED_REVERSE_INJECTION_PROPAGATION_H
