//
// Created by amrnasr on 16/10/2019.
//

#ifndef RTM_FRAMEWORK_FORWARD_COLLECTOR_H
#define RTM_FRAMEWORK_FORWARD_COLLECTOR_H

#include "component.h"
#include <skeleton/base/datatypes.h>

/*!
 * Forward Collector Interface. All concrete techniques for saving the forward
 * propagation or restoring the results from it should be implemented using this
 * interface.
 */
class ForwardCollector : public Component {
public:
  /*!
   * De-constructors should be overridden to ensure correct memory management.
   */
  virtual ~ForwardCollector(){};

  /*!
   * This function is called on each time step in the backward propagation and
   * should restore(2 propagation) or set (3 propagation -reverse)the
   * ForwardGrid variable to the suitable time step. The first call is expected
   * to make ForwardGrid current frame point to the frame corresponding to time
   * NT - 2.
   */
  virtual void FetchForward() = 0;

  /*!
   * This function is called on each time step in the forward propagation and
   * should store the current time step or save it in such a way that it can be
   * later obtained internally and stored in the ForwardGrid option. The first
   * call to this function happens when current frame in given grid is at
   * time 1. update pointers to save it in the right place in case of 2
   * propagation it will save in memory
   */
  virtual void SaveForward() = 0;

  /*!
   * Resets the grid pressures or allocates new frames for the backward
   * propagation. It should either free or keep track of the old frames. it is
   * called one time per shot before forward or backward propagation
   * @param forward_run
   * Indicates whether we are resetting the grid frame for the start of the
   * run(Forward) or between the propagations (Before Backward). True indicates
   * it is before the forward, false indicates before backward.
   */
  virtual void ResetGrid(bool forward_run) = 0;

  /*!
   * Getter to the internal GridBox to contain the current time step of the
   * forward propagation. This will be accessed to be able to correlate the
   * results in the backward propagation.
   * @return
   * A gridbox containing the appropriate values for the time that is implicitly
   * calculated using fetch forward. this function is called on each time step.
   */
  virtual GridBox *GetForwardGrid() = 0;
};
/*!example for the 3 propagation:
 * 1-Reset grid:
 * before forward when it is called it just resetting the array(set it by zeros)
 * 2-Save forward:
 * called at each time step do no thing as it is 3 propagation.
 * 3-Reset grid:
 * consider 2 GridBox one for backward and the other for reverse.
 * 4-Fetch forward:
 * called at each time step to do internal computation kernel calculations to
 *the corresponding time step(reverse propagation) 5-GetForwardGrid called at
 *each time step  to return the internal GridBox Note:correlation work on
 *current pressure.
 */
#endif // RTM_FRAMEWORK_FORWARD_COLLECTOR_H
