//
// Created by amrnasr on 16/10/2019.
//

#ifndef RTM_FRAMEWORK_MODEL_HANDLER_H
#define RTM_FRAMEWORK_MODEL_HANDLER_H
#include "component.h"
#include <skeleton/base/datatypes.h>
#include <skeleton/components/computation_kernel.h>

#include <string>
#include <vector>

using namespace std;
/*!
 * Model Handler Interface. All concrete techniques for loading the model or
 * setting the model should be implemented using this interface.
 */
class ModelHandler : public Component {
public:
  /*!
   * De-constructors should be overridden to ensure correct memory management.
   */
  virtual ~ModelHandler(){};

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
  virtual GridBox *ReadModel(vector<string> filenames,
                             ComputationKernel *kernel) = 0;

  /*!
   * All pre-processing needed to be done on the model before the beginning of
   * the reverse time migration, should be applied in this function.
   * @param kernel
   * The computation kernel to be used for first touch.
   */
  virtual void PreprocessModel(ComputationKernel *kernel) = 0;
};

#endif // RTM_FRAMEWORK_MODEL_HANDLER_H
