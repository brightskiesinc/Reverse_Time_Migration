//
// Created by amr on 09/12/2019.
//

#ifndef RTM_FRAMEWORK_COMPONENT_H
#define RTM_FRAMEWORK_COMPONENT_H

#include <skeleton/base/datatypes.h>
/*!
 * Component interface : all components in the framework needs to extend it.
 */
class Component {
public:
  /*!
   * De-constructors should be overridden to ensure correct memory management.
   */
  virtual ~Component(){};
  /*!
   * Sets the computation parameters to be used for the component.
   * @param parameters:Parameters of the simulation independent from the grid.
   */
  virtual void SetComputationParameters(ComputationParameters *parameters) = 0;

  /*!
   * Sets the grid box to operate on.
   * @param grid_box
   * The designated grid box to run operations on.
   */
  virtual void SetGridBox(GridBox *grid_box) = 0;
};

#endif // RTM_FRAMEWORK_COMPONENT_H
