/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   homogenous_model_handler.h
 * Author: mirnamoawad
 *
 * Created on October 22, 2019, 10:15 AM
 */

#ifndef HOMOGENOUS_MODEL_HANDLER_H
#define HOMOGENOUS_MODEL_HANDLER_H

#include <concrete-components/data_units/acoustic_second_grid.h>
#include <concrete-components/data_units/staggered_grid.h>
#include <skeleton/components/computation_kernel.h>
#include <skeleton/components/model_handler.h>
#include <skeleton/helpers/memory_allocation/memory_allocator.h>
class HomogenousModelHandler : public ModelHandler {
public:
  ~HomogenousModelHandler() override;

  GridBox *ReadModel(vector<string> filenames,
                     ComputationKernel *computational_kernel) override;

  void PreprocessModel(ComputationKernel *computational_kernel) override;

  void SetComputationParameters(ComputationParameters *parameters) override;

  void SetGridBox(GridBox *grid_box) override;

  HomogenousModelHandler(bool is_staggered);

  void SetupWindow() override;

private:
  ComputationParameters *parameters;
  GridBox *grid_box;
  bool is_staggered;
  static void GetSuitableDt(float dx, float dz, float dy, float *dt,
                            float *coeff, int max, int half_length,
                            float dt_relax);
  float SetModelField(float *field, vector<float> &val, int nx, int nz, int ny);
};
#endif /* HOMOGENOUS_MODEL_HANDLER_H */
