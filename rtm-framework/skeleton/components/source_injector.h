//
// Created by amrnasr on 16/10/2019.
//

#ifndef RTM_FRAMEWORK_SOURCE_INJECTOR_H
#define RTM_FRAMEWORK_SOURCE_INJECTOR_H

#include "component.h"
#include <skeleton/base/datatypes.h>

/*!
 * Source Injector Interface. All concrete techniques for source injection
 * should be implemented using this interface.
 */
/*! General note:virtual function =0 that means it isnot implemented yet and
 * should be implemented in the inherited class*/
class SourceInjector : public Component {
public:
  /*!
   * De-constructors should be overridden to ensure correct memory management.
   */
  virtual ~SourceInjector(){};

  /*!
   * Sets the source point to apply the injection to it.
   * @param source_point
   * The designated source point.
   */
  virtual void SetSourcePoint(Point3D *source_point) = 0;

  /*!
   * Function to apply source injection to the wavefield(s). It should inject
   * the current frame in our grid with the appropriate value. It should be
   * responsible of controlling when to stop the injection.
   * @param time_step
   * The time step corresponding to the current frame. This should be 1-based,
   * meaning the first frame that should be injected with our source should have
   * a time_step equal to 1.
   */
  virtual void ApplySource(uint time_step) = 0;

  /*!
   * Function to get the timestep that the source injection would stop after.
   * @return
   * An unsigned integer indicating the time step at which the source injection
   * would stop.
   */
  virtual uint GetCutOffTimestep() = 0;
};

#endif // RTM_FRAMEWORK_SOURCE_INJECTOR_H
