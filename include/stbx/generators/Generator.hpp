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

#ifndef SEISMIC_TOOLBOX_GENERATORS_GENERATOR_HPP
#define SEISMIC_TOOLBOX_GENERATORS_GENERATOR_HPP

#include <bs/base/configurations/concrete/JSONConfigurationMap.hpp>
#include <operations/common/DataTypes.h>
#include <operations/components/Components.hpp>
#include <operations/engine-configurations/concrete/RTMEngineConfigurations.hpp>
#include <operations/engine-configurations/concrete/ModellingEngineConfigurations.hpp>
#include <operations/engines/concrete/RTMEngine.hpp>
#include <operations/engines/concrete/ModellingEngine.hpp>
#include <operations/helpers/callbacks/primitive/CallbackCollection.hpp>

#include <stbx/generators/primitive/ConfigurationsGenerator.hpp>
#include <stbx/writers/Writers.h>
#include <stbx/agents/Agents.h>

#include <prerequisites/libraries/nlohmann/json.hpp>

#include <map>

namespace stbx {
    namespace generators {

        class Generator {
        public:
            /**
             * @brief Constructor.
             */
            explicit Generator(const nlohmann::json &aMap);

            /**
             * @brief Destructor ensures correct memory management.
             */
            ~Generator() = default;

            /**
             * @brief Extracts algorithm from system configurations file
             * to generate appropriate Engine instance.
             * @return Engine                    Engine instance
             */
            operations::engines::Engine *
            GenerateEngine(const std::string &aWritePath);

            /**
             * @brief Extracts algorithm from system configurations file
             * to generate appropriate Engine Configuration instance.
             * @return EngineConfiguration       EngineConfiguration instance
             */
            operations::configurations::EngineConfigurations *
            GenerateEngineConfiguration(const std::string &aWritePath);

            /**
             * @brief Extracts of Modelling Engine Configuration
             * component from parsed map  and returns Configuration instance.
             * @return ModellingEngineConfiguration instance
             */
            operations::configurations::ModellingEngineConfigurations *
            GenerateModellingEngineConfiguration(const std::string &aWritePath);

            /**
             * @brief Extracts of Engine Configuration
             * component from parsed map and returns Writer instance.
             * @return EngineConfiguration       EngineConfiguration instance
             */
            operations::configurations::RTMEngineConfigurations *
            GenerateRTMConfiguration(const std::string &aWritePath);

            /**
             * @brief Extracts of Callbacks component from parsed map
             * and returns CallbackCollection instance.
             * @return CallbackCollection       CallbackCollection instance
             */
            operations::helpers::callbacks::CallbackCollection *
            GenerateCallbacks(const std::string &aWritePath);

            /**
             * @brief Extracts of Computation Parameters
             * component from parsed map and returns Writer instance.
             * @return ComputationParameters       ComputationParameters instance
             */
            operations::common::ComputationParameters *
            GenerateParameters();

            /**
             * @brief Extracts of Agent component from parsed map
             * and returns Writer instance.
             * @return Agent       Agent instance
             */
            agents::Agent *
            GenerateAgent();

            /**
             * @brief Extracts of Writer component from parsed map
             * and returns Writer instance.
             * @return Writer       Writer instance
             */
            writers::Writer *
            GenerateWriter();

            /**
             * @brief Extracts timer properties from map and returns timer configuration.
             * @return JSONConfigurationMap: Timer configuration map
             */
            bs::base::configurations::ConfigurationMap *
            GenerateTimerConfiguration();

        private:
            /// Map that holds configurations key value pairs
            nlohmann::json mMap;
            EQUATION_ORDER mOrder;
            GRID_SAMPLING mSampling;
            APPROXIMATION mApproximation;
            ConfigurationsGenerator *mConfigurationsGenerator;
        };

    }//namespace generators
}//namespace stbx

#endif //SEISMIC_TOOLBOX_GENERATORS_GENERATOR_HPP
