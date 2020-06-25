//
// Created by amrnasr on 16/10/2019.
//

#ifndef RTM_FRAMEWORK_ENGINE_CONFIGURATION_H
#define RTM_FRAMEWORK_ENGINE_CONFIGURATION_H

#include <skeleton/components/boundary_manager.h>
#include <skeleton/components/computation_kernel.h>
#include <skeleton/components/correlation_kernel.h>
#include <skeleton/components/forward_collector.h>
#include <skeleton/components/model_handler.h>
#include <skeleton/components/modelling/modelling_configuration_parser.h>
#include <skeleton/components/modelling/trace_writer.h>
#include <skeleton/components/source_injector.h>
#include <skeleton/components/trace_manager.h>
/*!General note:Whether you need a destructor is NOT determined by whether you
 * use a struct or class. The deciding factor is whether the struct/class has
 * acquired resources that must be released explicitly when the life of the
 * object ends. If the answer to the question is yes, then you need to implement
 * a destructor. Otherwise, you don't need to implement it.
 */

// Struct that will contain pointers to concrete implementations of each
// component to be used in the RTM framework engine.
typedef struct EngineConfiguration {
  ForwardCollector *forward_collector;
  ModelHandler *model_handler;
  BoundaryManager *boundary_manager;
  SourceInjector *source_injector;
  ComputationKernel *computation_kernel;
  CorrelationKernel *correlation_kernel;
  TraceManager *trace_manager;

  /*!shot_start_id and shot_end_id are to support the different formats,for
   * example in segy, you might have a file that has shots 0 to 200 while you
   * only want to work on shots between 100-150 so in this case:
   * shot_start_id=100 and shot_end_id=150
   * so those just specify the starting shot id (inclusive) and the ending shot
   * id (exclusive)
   */
  uint sort_min;
  uint sort_max;
  string sort_key;

  // the velocity and density files
  vector<string> model_files;

  // the traces files:different files each file contains the traces for one shot
  // that may be output from the modeling engine
  vector<string> trace_files;

  /*!Destructor for correct destroying of the pointers*/
  ~EngineConfiguration() {
    delete forward_collector;
    delete model_handler;
    delete boundary_manager;
    delete source_injector;
    delete computation_kernel;
    delete correlation_kernel;
    delete trace_manager;
  }
} EngineConfiguration;

// Struct that will contain pointers to concrete implementations of some of the
// components of RTM framework to be used in modelling engine
typedef struct ModellingEngineConfiguration {
  /*!the ModellingEngineConfiguration is only used for modelling
   * and it doesn't have a forward collector
   * Because we don't need to store the forward propagation.
   * If we are modeling then we only do forward and store the traces while
   * propagating using the (TraceWriter) component we store the traces for each
   * shot in the trace file.(each shot's traces  in a different file)
   */
  ModelHandler *model_handler;
  BoundaryManager *boundary_manager;
  SourceInjector *source_injector;
  ComputationKernel *computation_kernel;
  TraceWriter *trace_writer;
  ModellingConfigurationParser *modelling_configuration_parser;

  // these are the velocity and density files
  vector<string> model_files;

  // the file used to parse the modeling configuration parameters from
  string modelling_configuration_file;

  // the file is used to store the traces for each shot
  // we have one trace file for each shot and it contains all the shot's traces
  string trace_file;

  /*!Destructor for correct destroying of the pointers*/
  ~ModellingEngineConfiguration() {
    delete model_handler;
    delete boundary_manager;
    delete source_injector;
    delete computation_kernel;
    delete trace_writer;
    delete modelling_configuration_parser;
  }
  /*!Example of modeling(one shot)
   * in the Modeling by giving it a velocity file in (model_files) and by using
   * the parameters come from the parsing of (modelling_configuration_file) ,we
   * do only the forward propagation using the concrete components of RTM
   * defined here and generate the pressure also Record at each time step the
   * pressure at the surface in the places we want to have receivers on using
   * (trace_writer) and put the output in (trace_file)
   */
  /*!Example of modeling(different shot)
   * the same for one shot except for the generation of the the different traces
   * files,they can be generated in two ways 1-run the acoustic_modeller
   * different times each with different shot location and use different trace
   * file name for each shot/run 2-run the script in data/run_modeller.sh and
   * give it the range you want to start(inclusive),end(exclusive) and increment
   * the shots by in the (i) iterator and it will automatically runs the
   * acoustic modeler different times for each shot location and generates
   * output trace files named (shot_i.trace) where (i)(iterator) represents the
   * shot location in x
   */
} ModellingEngineConfiguration;

/*!Example of rtm_engine
 *1- in case of one shot used: take the output of the modelling engine
 *(trace_file) as an input and the velocity file and generates the results 2-in
 *case of different shots used:take the output of the modelling engine
 *(trace_files) as a vector as input and the velocity file and generates the
 *results
 */
#endif // RTM_FRAMEWORK_ENGINE_CONFIGURATION_H
