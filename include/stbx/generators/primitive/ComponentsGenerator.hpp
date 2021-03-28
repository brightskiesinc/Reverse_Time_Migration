//
// Created by marwan-elsafty on 18/01/2021.
//

#ifndef STBX_GENERATORS_COMPONENTS_COMPONENTS_GENERATOR
#define STBX_GENERATORS_COMPONENTS_COMPONENTS_GENERATOR

#include <operations/configurations/concrete/JSONConfigurationMap.hpp>
#include <operations/common/DataTypes.h>
#include <operations/components/Components.hpp>

namespace stbx {
    namespace generators {

        class ComponentsGenerator {
        public:
            explicit ComponentsGenerator(const nlohmann::json &aMap,
                                         EQUATION_ORDER aOrder,
                                         GRID_SAMPLING aSampling,
                                         APPROXIMATION aApproximation);

            ~ComponentsGenerator() = default;

            operations::components::ComputationKernel *
            GenerateComputationKernel();

            operations::components::ModelHandler *
            GenerateModelHandler();

            operations::components::SourceInjector *
            GenerateSourceInjector();

            operations::components::BoundaryManager *
            GenerateBoundaryManager();

            operations::components::ForwardCollector *
            GenerateForwardCollector(const std::string &write_path);

            operations::components::MigrationAccommodator *
            GenerateMigrationAccommodator();

            operations::components::TraceManager *
            GenerateTraceManager();

            operations::components::ModellingConfigurationParser *
            GenerateModellingConfigurationParser();

            operations::components::TraceWriter *
            GenerateTraceWriter();


        private:
            operations::configuration::JSONConfigurationMap *
            TruncateMap(const std::string &aComponentName);

            nlohmann::json
            GetWaveMap();

            void CheckFirstOrder();

        private:
            /// Map that holds configurations key value pairs
            nlohmann::json mMap;
            EQUATION_ORDER mOrder;
            GRID_SAMPLING mSampling;
            APPROXIMATION mApproximation;
        };
    }// namespace generators
}//namesapce stbx


#endif //STBX_GENERATORS_COMPONENTS_COMPONENTS_GENERATOR
