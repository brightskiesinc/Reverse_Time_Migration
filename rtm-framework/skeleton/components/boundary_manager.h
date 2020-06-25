//
// Created by amrnasr on 16/10/2019.
//

#ifndef RTM_FRAMEWORK_BOUNDARY_MANAGER_H
#define RTM_FRAMEWORK_BOUNDARY_MANAGER_H

#include "component.h"
#include <skeleton/base/datatypes.h>
/*!
 * Boundary Manager Interface. All concrete techniques for absorbing boundary
 * conditions should be implemented using this interface.
 */
class BoundaryManager : public Component {
public:
  /*!
   * De-constructors should be overridden to ensure correct memory management.
   */
  virtual ~BoundaryManager(){};
  /*!
   * This function will be responsible of applying the absorbing boundary
   * condition on the current pressure frame. When provided with a kernel_id ,
   * this would lead to applying the boundaries on the designated kernel_id
   * kernel_id == 0 ---> Pressure
   * kernel_id == 1 ---> particle velocity.
   */
  /*!virtual function =0 that means it  not implemented yet and should be
   * implemented in the inherited class*/
  virtual void ApplyBoundary(uint kernel_id = 0) = 0;
  /*!
   * Extends the velocities/densities to the added boundary parts to the
   * velocity/density of the model appropriately. This is only called once after
   * the initialization of the model.
   */
  virtual void ExtendModel() = 0;
  /*!
   * Extends the velocities/densities to the added boundary parts to the
   * velocity/density of the model appropriately. This is called repeatedly with
   * before the forward propagation of each shot.
   */
  virtual void ReExtendModel() = 0;
  /*!
   * Adjusts the velocity/density of the model appropriately for the backward
   * propagation. Normally, but not necessary, this means removing the top
   * absorbing boundary layer. This is called repeatedly with before the
   * backward propagation of each shot.
   */
  virtual void AdjustModelForBackward() = 0;
};

#endif // RTM_FRAMEWORK_BOUNDARY_MANAGER_H
