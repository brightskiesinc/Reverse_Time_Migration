//
// Created by amrnasr on 21/10/2019.
//

#ifndef RTM_FRAMEWORK_DUMMYCOMPUTATIONKERNEL_H
#define RTM_FRAMEWORK_DUMMYCOMPUTATIONKERNEL_H

#include <skeleton/components/computation_kernel.h>
/*!inherited from the Computation Kernel class which All concrete techniques for
 * solving the wave equations should be implemented using it
 */
class DummyComputationKernel : public ComputationKernel {
public:
  /*!
   * De-constructors should be overridden to ensure correct memory management.
   */
  ~DummyComputationKernel() override;

  /*!
   * This function should solve the wave equation. It calculates the next
   * time-step from previous and current frames. It should also update the
   * GridBox structure so that after the function call, the GridBox structure
   * current frame should point to the newly calculated result, the previous
   * frame should point to the current frame at the time of the function call.
   * The next frame should point to the previous frame at the time of the
   * function call.
   */
  void Step() override;

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
  void FirstTouch(float *ptr, uint nx, uint nz, uint ny) override;

  /*!
   * Sets the computation parameters to be used for the component.
   * @param parameters:Parameters of the simulation independent from the Grid.
   */
  void SetComputationParameters(ComputationParameters *parameters) override;
  /*!
   * Sets the grid box to operate on.
   * @param grid_box :The designated grid box to run operations on.
   */
  void SetGridBox(GridBox *grid_box) override;
};

#endif // RTM_FRAMEWORK_DUMMYCOMPUTATIONKERNEL_H
