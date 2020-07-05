//
// Created by ingy on 1/26/20.
//

#include <IO/io_manager.h>
#include <Segy/segy_io_manager.h>
#include <concrete-components/data_units/acoustic_second_grid.h>
#include <datatypes.h>
#include <skeleton/components/computation_kernel.h>
#include <skeleton/components/model_handler.h>
#include <skeleton/helpers/memory_allocation/memory_allocator.h>

#ifndef ACOUSTIC2ND_RTM_SEGY_MODEL_HANDLER_H
#define ACOUSTIC2ND_RTM_SEGY_MODEL_HANDLER_H

class SeismicModelHandler : public ModelHandler {

public:
  SeismicModelHandler(bool is_staggered);

  ~SeismicModelHandler() override;

  GridBox *ReadModel(vector<string> filenames,
                     ComputationKernel *computational_kernel) override;

  void PreprocessModel(ComputationKernel *computational_kernel) override;

  void SetComputationParameters(ComputationParameters *parameters) override;

  void SetGridBox(GridBox *grid_box) override;

  void SetupWindow() override;

private:
  IOManager *IO;
  SeIO *sio;
  ComputationParameters *parameters;
  AcousticSecondGrid *grid_box;
  bool is_staggered;
  static void GetSuitableDt(int ny, float dx, float dz, float dy, float *dt,
                            float *coeff, int max, int half_length,
                            float dt_relax);
};

#endif // ACOUSTIC2ND_RTM_SEGY_MODEL_HANDLER_H
