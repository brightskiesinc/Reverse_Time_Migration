//
// Created by zeyad-osama on 20/09/2020.
//

#ifndef OPERATIONS_LIB_ENGINE_CONFIGURATIONS_FWI_ENGINE_CONFIGURATION_HPP
#define OPERATIONS_LIB_ENGINE_CONFIGURATIONS_FWI_ENGINE_CONFIGURATION_HPP

#include "operations/engine-configurations/interface/EngineConfigurations.hpp"

namespace operations {
    namespace configuration {

        class FWIEngineConfigurations : public EngineConfigurations {
        public:
            ~FWIEngineConfigurations() override {
                /// @todo To be implemented;
            }
        };
    } //namespace configuration
} //namespace operations

#endif //OPERATIONS_LIB_ENGINE_CONFIGURATIONS_FWI_ENGINE_CONFIGURATION_HPP
