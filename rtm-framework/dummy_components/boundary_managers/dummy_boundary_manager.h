//
// Created by amrnasr on 21/10/2019.
//

#ifndef RTM_FRAMEWORK_DUMMY_BOUNDARY_MANAGER_H
#define RTM_FRAMEWORK_DUMMY_BOUNDARY_MANAGER_H

#include <skeleton/components/boundary_manager.h>

class DummyBoundaryManager : public BoundaryManager {
public:
  /*!
   * De-constructors should be overridden to ensure correct memory management.
   */
  ~DummyBoundaryManager() override;

  /*!
   * This function will be responsible of applying the absorbing boundary
   * condition on the current pressure frame. In case of forward propagation, it
   * will apply boundaries on top, bottom, leftmost and rightmost edges.
   * Otherwise, it will only apply it on bottom, leftmost and rightmost edges.
   */
  /*!this function is a virtual function and will be overriden here*/
  void ApplyBoundary(uint kernel_id = 0) override;

  /*!
   * Extends the velocities/densities to the added boundary parts to the
   * velocity/density of the model appropriately. This is only called once after
   * the initialization of the model.
   */
  /*!this function is a virtual function and will be overriden here*/

  void ExtendModel() override;
  /*!
   * Extends the velocities/densities to the added boundary parts to the
   * velocity/density of the model appropriately. This is called repeatedly with
   * before the forward propagation of each shot.
   */
  /*!this function is a virtual function and will be overriden here*/

  void ReExtendModel() override;
  /*!
   * Sets the computation parameters to be used for the component.
   * @param parameters:Parameters of the simulation independent from the grid.
   */
  /*!this function is a virtual function and will be overriden here*/
  void SetComputationParameters(ComputationParameters *parameters) override;

  /*!
   * Sets the grid box to operate on.
   * @param grid_box
   * The designated grid box to run operations on.
   */
  /*!this function is a virtual function and will be overriden here*/
  void SetGridBox(GridBox *grid_box) override;

  /*!
   * Adjusts the velocity/density of the model appropriately for the backward
   * propagation. Normally, but not necessary, this means removing the top
   * absorbing boundary layer. This is called repeatedly with before the
   * backward propagation of each shot.
   */
  /*!this function is a virtual function and will be overriden here*/
  void AdjustModelForBackward() override;
};

#endif // RTM_FRAMEWORK_DUMMY_BOUNDARY_MANAGER_H
