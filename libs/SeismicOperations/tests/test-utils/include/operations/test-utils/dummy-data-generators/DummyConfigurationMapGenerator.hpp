//
// Created by marwan on 11/01/2021.
//

#ifndef OPERATIONS_LIB_TEST_UTILS_DUMMY_DATA_GENERATORS_DUMMY_CONFIGURATION_MAP_GENERATOR_HPP
#define OPERATIONS_LIB_TEST_UTILS_DUMMY_DATA_GENERATORS_DUMMY_CONFIGURATION_MAP_GENERATOR_HPP

#include <operations/configurations/concrete/JSONConfigurationMap.hpp>

#include <libraries/nlohmann/json.hpp>

namespace operations {
    namespace testutils {

        configuration::JSONConfigurationMap *generate_average_case_configuration_map_wave();

    } //namespace testutils
} //namespace operations

#endif //OPERATIONS_LIB_TEST_UTILS_DUMMY_DATA_GENERATORS_DUMMY_CONFIGURATION_MAP_GENERATOR_HPP
