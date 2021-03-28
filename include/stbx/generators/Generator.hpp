//
// Created by marwan-elsafty on 23/11/2020.
//

#ifndef SEISMIC_TOOLBOX_GENERATORS_GENERATOR_HPP
#define SEISMIC_TOOLBOX_GENERATORS_GENERATOR_HPP

#include <operations/configurations/concrete/JSONConfigurationMap.hpp>
#include <operations/common/DataTypes.h>
#include <operations/components/Components.hpp>
#include <operations/engine-configurations/concrete/RTMEngineConfigurations.hpp>
#include <operations/engine-configurations/concrete/ModellingEngineConfigurations.hpp>
#include <operations/helpers/callbacks/primitive/CallbackCollection.hpp>

#include <stbx/generators/primitive/ConfigurationsGenerator.hpp>
#include <stbx/writers/Writers.h>
#include <stbx/agents/Agents.h>

#include <libraries/nlohmann/json.hpp>

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
             * @brief Extracts of Modelling Engine Configuration
             * component from parsed map  and returns Configuration instance.
             * @return ModellingEngineConfiguration instance
             */
            operations::configuration::ModellingEngineConfigurations *
            GenerateModellingEngineConfiguration(const std::string &aWritePath);

            /**
             * @brief Extracts of Engine Configuration
             * component from parsed map and returns Writer instance.
             * @return EngineConfiguration       EngineConfiguration instance
             */
            operations::configuration::RTMEngineConfigurations *
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
