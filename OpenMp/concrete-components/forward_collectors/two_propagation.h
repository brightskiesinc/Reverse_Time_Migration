#ifndef ACOUSTIC2ND_RTM_TWO_PROPAGATION_H
#define ACOUSTIC2ND_RTM_TWO_PROPAGATION_H

#include <concrete-components/data_units/acoustic_second_grid.h>
#include <concrete-components/forward_collectors/file_handler/file_handler.h>
#include <skeleton/components/computation_kernel.h>
#include <skeleton/components/forward_collector.h>
#include <skeleton/helpers/memory_allocation/memory_allocator.h>

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <unistd.h>

class TwoPropagation : public ForwardCollector {
private:
  AcousticSecondGrid *main_grid;
  AcousticSecondGrid *internal_grid;
  ComputationParameters *parameters;
  float *forward_pressure;
  float *temp_prev;
  float *temp_curr;
  float *temp_next;
  uint pressure_size;
  bool mem_fit;
  unsigned long long max_nt;
  unsigned int time_counter;
  string write_path;
  bool compression;
  float zfp_tolerance;
  int zfp_parallel;
  bool zfp_is_relative;

public:
  TwoPropagation(bool compression, string write_path,
                 float zfp_tolerance = 0.01f, int zfp_parallel = 1,
                 bool zfp_is_relative = false);
  void FetchForward(void) override;
  void SaveForward() override;
  void ResetGrid(bool forward_run) override;
  void SetComputationParameters(ComputationParameters *parameters) override;
  void SetGridBox(GridBox *grid_box) override;
  GridBox *GetForwardGrid() override;
  ~TwoPropagation() override;
};

#endif
