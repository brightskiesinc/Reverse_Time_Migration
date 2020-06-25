//
// Created by amr on 12/11/2019.
//

#include "modelling_engine.h"
#include <skeleton/helpers/memory_allocation/memory_allocator.h>

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

void printProgress(double percentage) {
  int val = (int)(percentage * 100);
  int lpad = (int)(percentage * PBWIDTH);
  int rpad = PBWIDTH - lpad;
  printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
  fflush(stdout);
}

/*!
 * getInstance():This is the so-called Singleton design pattern.
 * Its distinguishing feature is that there can only ever be exactly one
 * instance of that class and the pattern ensures that. The class has a private
 * constructor and a statically-created instance that is returned with the
 * getInstance method. You cannot create an instance from the outside and thus
 * get the object only through said method.Since instance is static in the
 * getInstance method it will retain its value between multiple invocations. It
 * is allocated and constructed some where before it's first used. E.g. in this
 * answer it seems like GCC initializes the static variable at the time the
 * function is first used.
 */

/*!
 * Constructor to start the modelling engine given the appropriate engine
 * configuration.
 * @param configuration
 * The configuration which will control the actual work of the engine.
 * @param parameters
 * The computation parameters that will control the simulations settings like
 * boundary length, order of numerical solution.
 */
ModellingEngine::ModellingEngine(ModellingEngineConfiguration *configuration,
                                 ComputationParameters *parameters) {
  // the modeling configuration of the class is that given as argument to the
  // constructor
  this->configuration = configuration;
  // create a new CallbackCollection()
  /*!
   * callbacks are like hooks that you can register function to be called in
   * specific events(etc. after the forward propagation, after each time step,
   * and so on...) this enables the user to add functionalities,take snapshots
   * or track state in different approaches. A callback is the class actually
   * implementing the functions to capture or track state like norm, images, csv
   * and so on. They are registered to a callback collection which is what links
   * them to the engine. The engine would inform the callback collection at the
   * time of each event, and the callback collection would then inform each
   * callback registered to it.
   */
  this->callbacks = new CallbackCollection();
  // the computation parameters of this class is that given as argument to the
  // constructor
  this->parameters = parameters;
  // get an instance of the Timer.
  this->timer = Timer::getInstance();
}

/*!
 * Constructor to start the modelling engine given the appropriate engine
 * configuration.
 * @param configuration
 * The configuration which will control the actual work of the engine.
 * @param parameters
 * The computation parameters that will control the simulations settings like
 * boundary length, order of numerical solution.
 * @param cbs
 * The callbacks registered to be called in the right time.
 */
ModellingEngine::ModellingEngine(ModellingEngineConfiguration *configuration,
                                 ComputationParameters *parameters,
                                 CallbackCollection *cbs) {
  // the modeling configuration of the class is that given as argument to the
  // constructor
  this->configuration = configuration;
  // the callback of the class is that given as argument to the constructor
  this->callbacks = cbs;
  // the computation parameters of this class is that given as argument to the
  // constructor.
  this->parameters = parameters;
  // get an instance of the Timer.
  this->timer = Timer::getInstance();
}

/*!
 * Run the initialization steps for the modelling engine.
 */
GridBox *ModellingEngine::Initialize() {
  // start the timer and give it the name of function (Engine::Initialize)
  this->timer->start_timer("Engine::Initialize");

  // set the ComputationParameters with the parameters given to the constructor
  // for all needed functions.
  this->configuration->trace_writer->SetComputationParameters(parameters);
  this->configuration->boundary_manager->SetComputationParameters(parameters);
  this->configuration->computation_kernel->SetComputationParameters(parameters);
  this->configuration->model_handler->SetComputationParameters(parameters);
  this->configuration->source_injector->SetComputationParameters(parameters);
  this->configuration->modelling_configuration_parser->SetComputationParameters(
      parameters);

  /*!ry to read the model using the ReadModel function
   * the model_files contain the velocity and density files
   * and using the computation kernel in the configuration struct
   */
  GridBox *grid = this->configuration->model_handler->ReadModel(
      this->configuration->model_files,
      this->configuration->computation_kernel);

  /*!
   * All pre-processing needed to be done on the model before the beginning of
   * the reverse time migration, should be applied in this function.
   * @param kernel
   * The computation kernel to be used for first touch.
   * Here it is the computation kernel in the configuration struct
   */
  this->configuration->model_handler->SetGridBox(grid);
  this->configuration->model_handler->PreprocessModel(
      this->configuration->computation_kernel);

  // set the GridBox with the parameters given to the constructor for all needed
  // functions.
  this->configuration->trace_writer->SetGridBox(grid);
  this->configuration->boundary_manager->SetGridBox(grid);
  this->configuration->computation_kernel->SetGridBox(grid);
  this->configuration->source_injector->SetGridBox(grid);
  this->configuration->modelling_configuration_parser->SetGridBox(grid);

  /*!
   * Extends the velocities/densities to the added boundary parts to the
   * velocity/density of the model appropriately. This is only called once after
   * the initialization of the model.
   */
  this->configuration->boundary_manager->ExtendModel();

  /*! this function is used to parse the modelling_configuration_file to get the
     parameters of ModellingConfiguration struct (parameters of modeling)
      * Parses a file with the proper format as the modelling configuration.
      * @param filepath
      * The file to be parsed.
      * @return
      * The parsed modelling configuration.
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
      *Example: this function may be used to read the file modelling.txt and
     parse it
      * in a proper format to the struct ModellingConfiguration
      */
  ModellingConfiguration model_conf =
      this->configuration->modelling_configuration_parser->ParseConfiguration(
          this->configuration->modelling_configuration_file,
          grid->grid_size.ny == 1);

  // getting the number of time steps for the grid from the model_conf struct
  grid->nt = int(model_conf.total_time / grid->dt);

  /*! this function is used to initialize the trace writer with the modeling
   configurations
   * stored in model_conf struct and use the trace_file in configuration as
   output file to put traces in
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
      * it is a trace file contains all the traces for only one sho defined by
   the source_point
      */
  this->configuration->trace_writer->InitializeWriter(
      &model_conf, this->configuration->trace_file);

  // after updating the model_conf struct by the data exist in the
  // modelling_configuration_file put its value in the modelling_configuration
  // struct of this class
  this->modelling_configuration = model_conf;

  // stop the timer of function named (Engine::Initialize)
  this->timer->stop_timer("Engine::Initialize");

  // return the grid with its updated values
  return grid;
}

/*!
 * Begin the forward propagation and recording of the traces.
 */
void ModellingEngine::Forward(GridBox *grid_box) {
  // start the timer and give it the name of function (Engine::Forward)
  this->timer->start_timer("Engine::Forward");

  /*!this function is used to set the source point for the source injector
   *function with the source_point that exist in the modelling_configuration of
   *this class Sets the source point to apply the injection to it.
   * @param source_point
   * The designated source point.
   */
  this->configuration->source_injector->SetSourcePoint(
      &this->modelling_configuration.source_point);

  cout << "Forward Propagation" << endl;
  int onePercent = grid_box->nt / 100 + 1;

  for (uint t = 1; t < grid_box->nt; t++) {
    /*!
     * Function to apply source injection to the wavefield(s). It should inject
     * the current frame in our grid with the appropriate value. It should be
     * responsible of controlling when to stop the injection.
     * @param time_step
     * The time step corresponding to the current frame. This should be 1-based,
     * meaning the first frame that should be injected with our source should
     * have a time_step equal to 1.
     */
    this->configuration->source_injector->ApplySource(t);

    /*!
     * This function should solve the wave equation. It calculates the next
     * time-step from previous and current frames. It should also update the
     * GridBox structure so that after the function call, the GridBox structure
     * current frame should point to the newly calculated result, the previous
     * frame should point to the current frame at the time of the function call.
     * The next frame should point to the previous frame at the time of the
     * function call.
     */
    this->configuration->computation_kernel->Step();

#ifndef NDEBUG
    // if Iam in the debug mode use the call back of AfterForwardStep and give
    // it the updated gridBox
    this->callbacks->AfterForwardStep(grid_box, t);
#endif
    // if Iam not in the debug mode (release mode) we use call the RecordTrace()
    /*!
     * Records the traces from the domain according to the configuration given
     * in the initialize function.
     */
    this->configuration->trace_writer->RecordTrace();
    if ((t % onePercent) == 0) {
      printProgress(((float)t) / grid_box->nt);
    }
  }
  printProgress(1);
  cout << " ... Done" << endl;
  // stop the timer of function named (Engine::Forward)
  this->timer->stop_timer("Engine::Forward");
}

/*!
 * Start the modelling process.
 */
void ModellingEngine::Model() {
  // start the timer and give it the name of function (Engine::Engine::Model)
  this->timer->start_timer("Engine::Model");
#ifndef NDEBUG
  // if Iam in the debug mode use the call back of BeforeInitialization and give
  // it our updated parameters
  this->callbacks->BeforeInitialization(parameters);
#endif
  // if Iam not in the debug mode(release mode) call the Initialize() of this
  // class and return the updated GridBox
  /*!
   * Run the initialization steps for the modelling engine.
   */
  GridBox *grid_box = this->Initialize();
#ifndef NDEBUG
  // if Iam in the debug mode use the call back of AfterInitialization and give
  // it our updated gridbox
  this->callbacks->AfterInitialization(grid_box);
#endif
  this->configuration->computation_kernel->SetBoundaryManager(
      this->configuration->boundary_manager);
  // if Iam not in the debug mode(release mode) call the ReExtendModel() of this
  // class
  /*!
   * Extends the velocities/densities to the added boundary parts to the
   * velocity/density of the model appropriately. This is called repeatedly with
   * before the forward propagation of each shot.
   */
  this->configuration->boundary_manager->ReExtendModel();
#ifndef NDEBUG
  // if Iam in the debug mode use the call back of BeforeForwardPropagation and
  // give it our updated gridbox
  this->callbacks->BeforeForwardPropagation(grid_box);
#endif
  // if Iam not in the debug mode(release mode) call the Forward function of
  // this class and give it our updated GridBox
  /*!
   * Begin the forward propagation and recording of the traces.
   */
  this->Forward(grid_box);
  // free the GridBOX
  mem_free(grid_box);
  // stop the timer of the function named(Engine::Engine::Model)
  this->timer->stop_timer("Engine::Model");
}
