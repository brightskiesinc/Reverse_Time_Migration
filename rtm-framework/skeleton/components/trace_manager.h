//
// Created by amrnasr on 16/10/2019.
//

#ifndef RTM_FRAMEWORK_TRACE_MANAGER_H
#define RTM_FRAMEWORK_TRACE_MANAGER_H

#include "component.h"
#include <skeleton/base/datatypes.h>

#include <string>
#include <vector>

using namespace std;
/*!
 * Trace Manager Interface. All concrete techniques for reading, pre-processing
 * and injecting the traces corresponding to a model should be implemented using
 * this interface.
 */
/*! General note:virtual function =0 that means it  not implemented yet and
 * should be implemented in the inherited class*/

class TraceManager : public Component {
public:
  /*!
   * De-constructors should be overridden to ensure correct memory management.
   */
  virtual ~TraceManager(){};

  /*!
   * Function that should read the traces for a specified shot, and form the
   * trace grid appropriately.
   * @param filenames
   * A vector of filenames containing all the shots files.
   * @param shot_number
   * The shot number or id of the shot that its traces are targeted to be read.
   * @param sort_key
   * The type of sorting to access the data in.
   */
  virtual void ReadShot(vector<string> filenames, uint shot_number, string sort_key) = 0;

  /*!
   * Function that should be possible for the pre-processing of the shot traces
   * already read. Pre-processing includes interpolation of the traces, any type
   * of muting or special pre-processing applied on the traces and setting the
   * number of time steps of the simulation appropriately. Any changes needed to
   * be applied on the meta data of the grid should be done.
   * @param cutOffTime
   * The time step at which the source injection ends.
   */
  virtual void PreprocessShot(uint cut_off_time) = 0;

  /*!
   * Function that injects traces into the current frame of our GridBox
   * according to the time step to be used. this function is used in the
   * backward propagation
   * @param time_step
   * The times_step of the current frame in our GridBox object. Starts from NT-1
   * with this being the time of the first trace(backward propagation starts
   * from NT-1)
   */
  virtual void ApplyTraces(uint time_step) = 0;

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
  virtual Point3D *GetSourcePoint() = 0;

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
  virtual Traces *GetTraces() = 0;
  /*!
   * Get the shots that you can be migrated from the data according to a min/max and type.
   * @param filenames
   * The files containing the traces.
   * @param min_shot
   * The minimum shot ID to allow.
   * @param max_shot
   * The maximum shot ID to allow.
   * @param type
   * Can be "CDP" or "CSR"
   * @return
   * The vector containing the shot IDs.
   */
  virtual vector<uint> GetWorkingShots(vector<string> filenames, uint min_shot, uint max_shot, string type) = 0;
};

#endif // RTM_FRAMEWORK_TRACE_MANAGER_H
