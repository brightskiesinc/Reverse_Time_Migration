//
// Created by amr on 11/11/2019.
//

#ifndef RTM_FRAMEWORK_TRACE_WRITER_H
#define RTM_FRAMEWORK_TRACE_WRITER_H

#include <skeleton/base/datatypes.h>
#include <skeleton/components/component.h>
#include <string>

// this class is used to Record at each time step the pressure at the surface in
// the places we want to have receivers on. can be considered as a hydrophone.
/*! General note:virtual function =0 that means it is not implemented yet and
 * should be implemented in the inherited class*/
class TraceWriter : public Component {
public:
  /*!
   * Virtual destroyer for correct destruction.
   */
  virtual ~TraceWriter(){};

  /*!
   * Initializes the trace writer with all needed data for it to be able to
   start recording traces
   * according the the given configuration.
   * @param modelling_config
   * The modelling configuration to be followed in recording the traces.
   * * ModellingConfiguration:Parameters needed for the modelling operation.
          typedef struct {
              // Starting point for the receivers.
              Point3D receivers_start;
              // The increment step of the receivers.
              Point3D receivers_increment;
              // The end point of the receivers exclusive.
              Point3D receivers_end;
              // The source point for the modelling.
              Point3D source_point;
              // The total time of the simulation in seconds.
              float total_time;
           } ModellingConfiguration;
   * @param output_filename
   * The output file to write the traces into.
   * it is a trace file contains all the traces for only one sho defined by the
   source_point
   */
  virtual void InitializeWriter(ModellingConfiguration *modelling_config,
                                std::string output_filename) = 0;

  /*!
   * Records the traces from the domain according to the configuration given in
   * the initialize function.
   */
  virtual void RecordTrace() = 0;
};

#endif // RTM_FRAMEWORK_TRACE_WRITER_H
