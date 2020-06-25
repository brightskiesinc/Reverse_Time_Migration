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

#include <CL/sycl.hpp>
#include <concrete-components/data_units/acoustic_second_grid.h>
#include <skeleton/components/computation_kernel.h>
#include <skeleton/components/model_handler.h>
#include <skeleton/helpers/memory_allocation/memory_allocator.h>

using namespace cl::sycl;

class HomogenousModelHandler : public ModelHandler {
public:
  ~HomogenousModelHandler() override;

  GridBox *ReadModel(vector<string> filenames,
                     ComputationKernel *computational_kernel) override;

  void PreprocessModel(ComputationKernel *computational_kernel) override;

  void SetComputationParameters(ComputationParameters *parameters) override;

  void SetGridBox(GridBox *grid_box) override;

private:
  ComputationParameters *parameters;
  AcousticSecondGrid *grid_box;
  static void get_suitable_dt(float dx, float dz, float dy, float *dt,
                              float *coeff, int max, int half_length,
                              float dt_relax);
};
#endif /* HOMOGENOUS_MODEL_HANDLER_H */
