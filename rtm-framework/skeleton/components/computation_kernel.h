//
// Created by amrnasr on 16/10/2019.
//

#ifndef RTM_FRAMEWORK_COMPUTATION_KERNEL_H
#define RTM_FRAMEWORK_COMPUTATION_KERNEL_H

#include "boundary_manager.h"
#include "component.h"
#include <skeleton/base/datatypes.h>
/*!
 * Computation Kernel Interface. All concrete techniques for solving the wave
 * equations should be implemented using this interface.
 */
class ComputationKernel : public Component {
protected:
  BoundaryManager *boundary_manager;

public:
  /*!
   * De-constructors should be overridden to ensure correct memory management.
   */
  virtual ~ComputationKernel(){};

  /*!
   * This function should solve the wave equation. It calculates the next
   * time-step from previous and current frames. It should also update the
   * GridBox structure so that after the function call, the GridBox structure
   * current frame should point to the newly calculated result, the previous
   * frame should point to the current frame at the time of the function call.
   * The next frame should point to the previous frame at the time of the
   * function call.
   */
  virtual void Step() = 0;

  /*!
   * Set kernel boundary manager to be used and called internally.
   * @param boundary_manager
   * The boundary manager to be used.
   */
  void SetBoundaryManager(BoundaryManager *boundary_manager) {
    this->boundary_manager = boundary_manager;
  }
  /*!
   * Applies first touch initialization by accessing the given pointer in the
   * same way it will be accessed in the step function, and initializing the
   * values to 0.
   * @param ptr
   * The pointer that will be accessed for the first time.
   * @param nx
   * # of elements in x direction.
   * @param nz
   * # of elements in z direction.
   * @param ny
   * # of elements in y direction.
   */
  virtual void FirstTouch(float *ptr, uint nx, uint nz, uint ny) = 0;
};

#endif // RTM_FRAMEWORK_COMPUTATION_KERNEL_H
