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

#ifndef STBX_GENERATORS_COMPONENTS_COMPONENTS_GENERATOR
#define STBX_GENERATORS_COMPONENTS_COMPONENTS_GENERATOR

#include <bs/base/configurations/concrete/JSONConfigurationMap.hpp>
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

            operations::components::TraceWriter *
            GenerateTraceWriter();


        private:
            bs::base::configurations::JSONConfigurationMap *
            TruncateMap(const std::string &aComponentName);

            nlohmann::json
            GetWaveMap();

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
