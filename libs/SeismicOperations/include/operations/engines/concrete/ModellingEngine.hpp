/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of SeismicToolbox.
 *
 * SeismicToolbox is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SeismicToolbox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef OPERATIONS_LIB_ENGINES_MODELLING_ENGINE_HPP
#define OPERATIONS_LIB_ENGINES_MODELLING_ENGINE_HPP

#include <operations/engines/interface/Engine.hpp>

#include <operations/engine-configurations/concrete/ModellingEngineConfigurations.hpp>
#include <operations/helpers/callbacks/primitive/CallbackCollection.hpp>


namespace operations {
    namespace engines {
        /**
         * @brief The Modelling engine responsible of using the different
         * components to model the needed algorithm.
         */
        class ModellingEngine : public Engine {
        public:
            /**
             * @brief Constructor to start the modelling engine given the appropriate engine
             * configuration.
             *
             * @param[in] apConfiguration
             * The configuration which will control the actual work of the engine.
             *
             * @param[in] apParameters
             * The computation parameters that will control the simulations settings like
             * boundary length, order of numerical solution.
             */
            ModellingEngine(configurations::ModellingEngineConfigurations *apConfiguration,
                            common::ComputationParameters *apParameters);

            /**
             * @brief Constructor to start the modelling engine given the appropriate engine
             * configuration.
             *
             * @param[in] apConfiguration
             * The configuration which will control the actual work of the engine.
             *
             * @param[in] apParameters
             * The computation parameters that will control the simulations settings like
             * boundary length, order of numerical solution.
             *
             * @param[in] apCallbackCollection
             * The callbacks registered to be called in the right time.
             */
            ModellingEngine(configurations::ModellingEngineConfigurations *apConfiguration,
                            common::ComputationParameters *apParameters,
                            helpers::callbacks::CallbackCollection *apCallbackCollection);

            /**
             * @brief Destructors should be overridden to ensure correct memory management.
             */
            ~ModellingEngine() override;

            /**
             * @brief Run the initialization steps for the modelling engine.
             *
             * @return[out] GridBox
             */
            dataunits::GridBox *
            Initialize() override;

            /**
             * @brief The function that filters and returns all possible
             * shot ID in a vector to be fed to the migrate method.
             *
             * @return[out]
             * A vector containing all unique shot IDs.
             */
            std::vector<uint>
            GetValidShots() override;

            /**
             * @brief The migration function that will apply the
             * reverse time migration process and produce the results needed.
             *
             * @param[in] shot_list
             * A vector containing the shot IDs to be migrated.
             */
            void
            MigrateShots(std::vector<uint> shot_numbers, dataunits::GridBox *apGridBox) override;

            /**
             * @brief The migration function that will apply the
             * modeling process and produce the results needed.
             *
             * @param[in] shot_id
             * Shot IDs to be modeled.
             */
            void
            MigrateShots(uint shot_id, dataunits::GridBox *apGridBox);

            /**
             * @brief Finalizes and terminates all processes
             *
             * @return[out]
             * A float pointer to the array containing the final correlation result.
             */
            dataunits::MigrationData *
            Finalize(dataunits::GridBox *apGridBox) override;

        private:
            /**
             * @brief Applies the forward propagation using the different
             * components provided in the configuration.
             */
            void
            Forward(dataunits::GridBox *apGridBox, uint shot_id);

        private:
            ///The configuration containing the actual components to be used in the process.
            configurations::ModellingEngineConfigurations *mpConfiguration;
        };
    } //namespace engines
} //namespace operations

#endif // OPERATIONS_LIB_ENGINES_MODELLING_ENGINE_HPP
