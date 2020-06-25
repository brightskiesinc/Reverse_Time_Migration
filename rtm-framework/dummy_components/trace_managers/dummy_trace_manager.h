//
// Created by amrnasr on 21/10/2019.
//

#ifndef RTM_FRAMEWORK_DUMMY_TRACE_MANAGER_H
#define RTM_FRAMEWORK_DUMMY_TRACE_MANAGER_H

#include <skeleton/components/trace_manager.h>

class DummyTraceManager : public TraceManager {
private:
  GridBox *dummy_grid;
  Point3D source_point;

public:
  /*!
   * De-constructors should be overridden to ensure correct memory management
   * and is overridden here.
   */
  ~DummyTraceManager() override;

  /*!
   * Function that should read the traces for a specified shot, and form the
   * trace grid appropriately.
   * @param filenames
   * A vector of filenames containing all the shots files.
   * @param shot_number
   * The shot number or id of the shot that its traces are targeted to be read.
   */
  /*!this function is a virtual function and will be overridden here*/
  void ReadShot(vector<string> filenames, uint shot_number, string sort_key) override;

  /*!
   * Function that should be possible for the pre-processing of the shot traces
   * already read. Pre-processing includes interpolation of the traces, any type
   * of muting or special pre-processing applied on the traces and setting the
   * number of time steps of the simulation appropriately. Any changes needed to
   * be applied on the meta data of the grid should be done.
   * @param cutOffTime
   * The time step at which the source injection ends.
   */
  /*!this function is a virtual function and will be overridden here*/
  void PreprocessShot(uint cut_off_time) override;

  /*!
   * Function that injects traces into the current frame of our GridBox
   * according to the time step to be used. this function is used in the
   * backward propagation
   * @param time_step
   * The times_step of the current frame in our GridBox object. Starts from NT-1
   * with this being the time of the first trace(backward propagation starts
   * from NT-1)
   */
  /*!this function is a virtual function and will be overridden here*/
  void ApplyTraces(uint time_step) override;

  /*!
 * @return
 * Pointer containing the information and values of the traces to be injected.
 * Dimensions should be in this order as
 majority[time][y-dimension][x-dimension]. *Traces:Structure containing the
 available traces information.
 * typedef struct  {
             float *traces;
             uint num_receivers_in_x;
             uint num_receivers_in_y;
             uint trace_size_per_timestep;
             uint sample_nt;
            float sample_dt;
            } Traces;

 */
  /*!this function is a virtual function and will be overridden here*/
  Traces *GetTraces() override;

  /*!
   * Sets the computation parameters to be used for the component.
   * @param parameters:Parameters of the simulation independent from the grid.
   */
  /*!this function is a virtual function and will be overridden here*/
  void SetComputationParameters(ComputationParameters *parameters) override;

  /*!
   * Sets the grid box to operate on.
   * @param grid_box
   * The designated grid box to run operations on.
   */
  /*!this function is a virtual function and will be overridden here*/
  void SetGridBox(GridBox *grid_box) override;

  /*!
 * Getter to the property containing the shot location source point
 * of the current read traces. This value should be set in the ReadShot
 function.
 * @return
 * A pointer to a Point3D that indicates the source point.
 * Point3D is a Point coordinates in 3D.
         typedef struct {
                       uint x;
                       uint z;
                       uint y;
                       } Point3D;
 */
  /*!this function is a virtual function and will be overridden here*/
  Point3D *GetSourcePoint() override;

  vector<uint> GetWorkingShots(vector<string> filenames, uint min_shot, uint max_shot, string type) override;

};

#endif // RTM_FRAMEWORK_DUMMY_TRACE_MANAGER_H
