//
// Created by amrnasr on 21/10/2019.
//

#ifndef RTM_FRAMEWORK_DUMMY_SOURCE_INJECTOR_H
#define RTM_FRAMEWORK_DUMMY_SOURCE_INJECTOR_H

#include <skeleton/components/source_injector.h>

class DummySourceInjector : public SourceInjector {
public:
  /*!
   * De-constructors should be overridden to ensure correct memory management.
   */
  ~DummySourceInjector() override;

  /*!
   * Sets the source point to apply the injection to it.
   * @param source_point
   * The designated source point.
   */
  /*!this function is a virtual function and will be overriden here*/
  void ApplySource(uint time_step) override;

  /*!
   * Function to get the timestep that the source injection would stop after.
   * @return
   * An unsigned integer indicating the time step at which the source injection
   * would stop.
   */
  /*!this function is a virtual function and will be overriden here*/
  uint GetCutOffTimestep() override;

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
   * Sets the source point to apply the injection to it.
   * @param source_point
   * The designated source point.
   */
  /*!this function is a virtual function and will be overriden here*/
  void SetSourcePoint(Point3D *source_point) override;
};

#endif // RTM_FRAMEWORK_DUMMY_SOURCE_INJECTOR_H
