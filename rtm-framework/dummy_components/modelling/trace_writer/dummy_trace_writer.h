//
// Created by amr on 12/11/2019.
//

#ifndef RTM_FRAMEWORK_DUMMYTRACEWRITER_H
#define RTM_FRAMEWORK_DUMMYTRACEWRITER_H

#include <skeleton/components/modelling/trace_writer.h>

class DummyTraceWriter : public TraceWriter {
public:
  /*!
   * Virtual destroyer for correct destruction and is overridden here
   */
  ~DummyTraceWriter() override;
  /*!
   * Initializes the trace writer with all needed data for it to be able to
  start recording traces
   * according the the given configuration.
   * @param modelling_config
   * The modelling configuration to be followed in recording the traces.
   * ModellingConfiguration:Parameters needed for the modelling operation.
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
  /*!this function is a virtual function and will be overridden here*/
  void InitializeWriter(ModellingConfiguration *modelling_config,
                        std::string output_filename) override;

  /*!
   * Records the traces from the domain according to the configuration given in
   * the initialize function
   */
  /*!this function is a virtual function and will be overridden here*/
  void RecordTrace() override;

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
};

#endif // RTM_FRAMEWORK_DUMMYTRACEWRITER_H
