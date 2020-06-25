//
// Created by amrnasr on 21/10/2019.
//

#ifndef RTM_FRAMEWORK_DUMMY_FORWARD_COLLECTOR_H
#define RTM_FRAMEWORK_DUMMY_FORWARD_COLLECTOR_H

#include <skeleton/components/forward_collector.h>

class DummyForwardCollector : public ForwardCollector {
public:
  /*!
   * De-constructors should be overridden to ensure correct memory management.
   */
  ~DummyForwardCollector() override;
  /*!
   * This function is called on each time step in the backward propagation and
   * should restore(2 propagation) or set (3 propagation -reverse)the
   * ForwardGrid variable to the suitable time step. The first call is expected
   * to make ForwardGrid current frame point to the frame corresponding to time
   * NT - 2.
   */
  void FetchForward() override;

  /*!
   * This function is called on each time step in the forward propagation and
   * should store the current time step or save it in such a way that it can be
   * later obtained internally and stored in the ForwardGrid option. The first
   * call to this function happens when current frame in given grid is at
   * time 1. update pointers to save it in the right place in case of 2
   * propagation it will save in memory
   */
  void SaveForward() override;

  /*!
   * Resets the grid pressures or allocates new frames for the backward
   * propagation. It should either free or keep track of the old frames. it is
   * called one time per shot before forward or backward propagation
   * @param forward_run
   * Indicates whether we are resetting the grid frame for the start of the
   * run(Forward) or between the propagations (Before Backward). True indicates
   * it is before the forward, false indicates before backward.
   */
  void ResetGrid(bool forward_run) override;

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

  /*!
   * Getter to the internal GridBox to contain the current time step of the
   * forward propagation. This will be accessed to be able to correlate the
   * results in the backward propagation.
   * @return
   * A gridbox containing the appropriate values for the time that is implicitly
   * calculated using fetch forward. this function is called on each time step.
   */
  GridBox *GetForwardGrid() override;
};

#endif // RTM_FRAMEWORK_DUMMY_FORWARD_COLLECTOR_H
