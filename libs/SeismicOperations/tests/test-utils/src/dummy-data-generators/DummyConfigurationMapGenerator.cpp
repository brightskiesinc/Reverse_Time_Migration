//
// Created by marwan-elsafty on 05/01/2021.
//

#include <operations/test-utils/dummy-data-generators/DummyConfigurationMapGenerator.hpp>

using namespace operations::configuration;
using json = nlohmann::json;

namespace operations {
    namespace testutils {

        JSONConfigurationMap *generate_average_case_configuration_map_wave() {
            return new JSONConfigurationMap(R"(
                {
                    "wave": {
                        "physics": "acoustic",
                        "approximation": "isotropic",
                        "equation-order": "first",
                        "grid-sampling": "uniform"
                    }
                }
            )"_json);
        }

    } //namespace testutils
} //namespace operations
