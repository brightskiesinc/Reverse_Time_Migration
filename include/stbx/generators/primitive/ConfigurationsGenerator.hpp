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

#ifndef STBX_GENERATORS_CONFIGURATIONS_CONFIGURATIONS_GENERATOR_H
#define STBX_GENERATORS_CONFIGURATIONS_CONFIGURATIONS_GENERATOR_H

#include <operations/common/DataTypes.h>
#include <prerequisites/libraries/nlohmann/json.hpp>

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
            GetTraceFiles();

            uint GetTracesMin();

            uint GetTracesMax();

            std::string GetTracesSortKey();

        private:
            /// Map that holds configurations key value pairs
            nlohmann::json mMap;

        };
    }//namespace generators
}//namespace stbx

#endif //STBX_GENERATORS_CONFIGURATIONS_CONFIGURATIONS_GENERATOR_H

