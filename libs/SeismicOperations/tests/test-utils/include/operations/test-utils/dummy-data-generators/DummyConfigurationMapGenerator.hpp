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


#ifndef OPERATIONS_LIB_TEST_UTILS_DUMMY_DATA_GENERATORS_DUMMY_CONFIGURATION_MAP_GENERATOR_HPP
#define OPERATIONS_LIB_TEST_UTILS_DUMMY_DATA_GENERATORS_DUMMY_CONFIGURATION_MAP_GENERATOR_HPP

#include <bs/base/configurations/concrete/JSONConfigurationMap.hpp>

#include <prerequisites/libraries/nlohmann/json.hpp>

namespace operations {
    namespace testutils {

        bs::base::configurations::JSONConfigurationMap *generate_average_case_configuration_map_wave();

    } //namespace testutils
} //namespace operations

#endif //OPERATIONS_LIB_TEST_UTILS_DUMMY_DATA_GENERATORS_DUMMY_CONFIGURATION_MAP_GENERATOR_HPP
