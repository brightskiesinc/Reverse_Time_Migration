#ifndef ACOUSTIC2ND_RTM_REVERSE_PROPAGATION_H
#define ACOUSTIC2ND_RTM_REVERSE_PROPAGATION_H

#include <CL/sycl.hpp>
#include <concrete-components/data_units/acoustic_second_grid.h>
#include <cstdlib>
#include <cstring>
#include <skeleton/components/computation_kernel.h>
#include <skeleton/components/forward_collector.h>
#include <skeleton/helpers/memory_allocation/memory_allocator.h>

class ReversePropagation : public ForwardCollector {
private:
  AcousticSecondGrid *main_grid;
  AcousticSecondGrid *internal_grid;
  ComputationParameters *parameters;
  ComputationKernel *computation_kernel;
  sycl::queue *internal_queue;

public:
  ReversePropagation(ComputationKernel *kernel);
  void FetchForward(void) override;
  void SaveForward() override;
  void ResetGrid(bool forward_run) override;
  void SetComputationParameters(ComputationParameters *parameters) override;
  void SetGridBox(GridBox *grid_box) override;
  GridBox *GetForwardGrid() override;
  ~ReversePropagation() override;
};
#endif
