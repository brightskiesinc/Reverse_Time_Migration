//
// Created by amr on 12/11/2019.
//
#ifndef RTM_FRAMEWORK_MODELLING_ENGINE_H
#define RTM_FRAMEWORK_MODELLING_ENGINE_H

#include <skeleton/base/engine_configuration.h>
#include <skeleton/helpers/callbacks/callback_collection.h>
#include <skeleton/helpers/timer/timer.hpp>

class ModellingEngine {
private:
  /*!
   * Run the initialization steps for the modelling engine.
   */
  GridBox *Initialize();

  /*!
   * Begin the forward propagation and recording of the traces.
   */
  void Forward(GridBox *grid_box);

  /*!
   * The engine configuration.
   */
  ModellingEngineConfiguration *configuration;

  /*!
   * The callbacks to be used.
   */
  CallbackCollection *callbacks;

  /*!
   * The modelling configuration.
   */
  ModellingConfiguration modelling_configuration;

  /*!
   * The computations parameters.
   */
  ComputationParameters *parameters;

  /*!
   * The timer to used.
   */
  Timer *timer;

public:
  /*!
   * Constructor to start the modelling engine given the appropriate engine
   * configuration.
   * @param configuration
   * The configuration which will control the actual work of the engine.
   * @param parameters
   * The computation parameters that will control the simulations settings like
   * boundary length, order of numerical solution.
   */
  ModellingEngine(ModellingEngineConfiguration *configuration,
                  ComputationParameters *parameters);

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
  ModellingEngine(ModellingEngineConfiguration *configuration,
                  ComputationParameters *parameters, CallbackCollection *cbs);

  /*!
   * Start the modelling process.
   */
  void Model();
};
#endif // RTM_FRAMEWORK_MODELLING_ENGINE_H
