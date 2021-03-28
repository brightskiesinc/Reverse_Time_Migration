//
// Created by marwan-elsafty on 21/01/2021.
//

#ifndef STBX_GENERATORS_CONFIGURATIONS_CONFIGURATIONS_GENERATOR_H
#define STBX_GENERATORS_CONFIGURATIONS_CONFIGURATIONS_GENERATOR_H

#include <operations/engine-configurations/concrete/RTMEngineConfigurations.hpp>
#include <operations/common/DataTypes.h>

#include <libraries/nlohmann/json.hpp>

#include <map>

namespace stbx {
    namespace generators {
        class ConfigurationsGenerator {
        public:
            explicit ConfigurationsGenerator(nlohmann::json &aMap);

            ~ConfigurationsGenerator() = default;

            PHYSICS
            GetPhysics();

            EQUATION_ORDER
            GetEquationOrder();

            GRID_SAMPLING
            GetGridSampling();

            APPROXIMATION
            GetApproximation();

            std::map<std::string, std::string>
            GetModelFiles();

            std::vector<std::string>
            GetTraceFiles(operations::configuration::RTMEngineConfigurations *aConfiguration);

            std::string
            GetModellingFile();

            std::string
            GetOutputFile();


        private:
            /// Map that holds configurations key value pairs
            nlohmann::json mMap;

        };
    }//namespace generators
}//namespace stbx

#endif //STBX_GENERATORS_CONFIGURATIONS_CONFIGURATIONS_GENERATOR_H

