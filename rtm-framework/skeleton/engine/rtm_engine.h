//
// Created by amrnasr on 16/10/2019.
//

#ifndef RTM_FRAMEWORK_RTM_ENGINE_H
#define RTM_FRAMEWORK_RTM_ENGINE_H

#include <skeleton/base/engine_configuration.h>
#include <skeleton/base/migration_data.h>
#include <skeleton/helpers/callbacks/callback_collection.h>
#include <skeleton/helpers/timer/timer.hpp>
#include <vector>

/*!
 * The RTM engine responsible of using the different components to apply the
 * reverse time migration.
 */

class RTMEngine {
private:
  /*!
   * The configuration containing the actual components to be used in the
   * process.
   */
  EngineConfiguration *configuration;
  /*!
   * Callback collection to be called when not in release mode.
   */
  CallbackCollection *callbacks;
  /*!
   * The computations paramters.
   */
  ComputationParameters *parameters;
  /*!
   * The timer to use.
   */
  Timer *timer;
  /*!
   * Applies the forward propagation using the different components provided in
   * the configuration.
   */
  void Forward(GridBox *grid_box);
  /*!
   * Applies the backward propagation using the different components provided in
   * the configuration.
   */
  void Backward(GridBox *grid_box);
  /*!
   * Initializes our domain model.
   */
  GridBox *Initialize();

public:
  /*!
   * Constructor for the RTM engine giving it the configuration needed.
   * @param configuration
   * The engine configuration that should be used for the RTM engine.
   * @param parameters
   * The computation parameters that will control the simulations settings like
   * boundary length, order of numerical solution.
   */

  RTMEngine(EngineConfiguration *configuration,
            ComputationParameters *parameters);
  /*!
   * Constructor for the RTM engine giving it the configuration needed.
   * @param configuration
   * The engine configuration that should be used for the RTM engine.
   * @param parameters
   * The computation parameters that will control the simulations settings like
   * boundary length, order of numerical solution.
   * @param cbs
   * The callback collection to be called throughout the execution if in debug
   * mode.
   */
  RTMEngine(EngineConfiguration *configuration,
            ComputationParameters *parameters, CallbackCollection *cbs);
  /*!
   * The migration function that will apply the reverse time migration process
   * and produce the results needed.
   * @param shot_list
   * A vector containing the shot IDs to be migrated.
   * @return
   * A float pointer to the array containing the final correlation result.
   */

  MigrationData *Migrate(vector<uint> shot_list);
  /*!
   * The function that filters and returns all possible shot ID in a vector to be fed to the migrate method.
   * @return
   * A vector containing all unique shot IDs.
   */
  vector<uint> GetValidShots();
};

#endif // RTM_FRAMEWORK_RTM_ENGINE_H
