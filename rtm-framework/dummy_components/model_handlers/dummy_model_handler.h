//
// Created by amrnasr on 21/10/2019.
//

#ifndef RTM_FRAMEWORK_DUMMY_MODEL_HANDLER_H
#define RTM_FRAMEWORK_DUMMY_MODEL_HANDLER_H

#include <skeleton/components/model_handler.h>

class DummyModelHandler : public ModelHandler {
public:
  /*!
   * De-constructors should be overridden to ensure correct memory management.
   */
  ~DummyModelHandler() override;

  /*!
   * This function should create a new GridBox with all its frames (previous ,
   * current , next) allocated in memory. The velocity/density is loaded into
   * the frame. This should account for the allocation of the boundaries around
   * our domain(top, bottom, left, right). It should set all possible GridBox
   * properties to be ready for the actual computations.
   * @param filenames
   * The filenames vector of the files containing the model, the first filename
   * in the vector should be the velocity file, the second should be the density
   * file name.
   * @param kernel
   * The computation kernel to be used for first touch.
   * @return
   * A GridBox object that was allocated, and setup appropriately.
   */
  GridBox *ReadModel(vector<string> filenames,
                     ComputationKernel *kernel) override;

  /*!
   * All pre-processing needed to be done on the model before the beginning of
   * the reverse time migration, should be applied in this function.
   * @param kernel
   * The computation kernel to be used for first touch.
   */
  void PreprocessModel(ComputationKernel *kernel) override;

  /*!
   * Sets the computation parameters to be used for the component.
   * @param parameters:Parameters of the simulation independent from the block.
   */
  void SetComputationParameters(ComputationParameters *parameters) override;

  /*!
   * Sets the grid box to operate on.
   * @param grid_box :The designated grid box to run operations on.
   */
  void SetGridBox(GridBox *grid_box) override;
};

#endif // RTM_FRAMEWORK_DUMMY_MODEL_HANDLER_H
