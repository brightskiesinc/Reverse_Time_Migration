//
// Created by zeyad-osama on 29/11/2020.
//

#ifndef THOTH_CONFIGURATIONS_CONFIGURABLE_HPP
#define THOTH_CONFIGURATIONS_CONFIGURABLE_HPP

#include <thoth/configurations/interface/ConfigurationMap.hpp>

namespace thoth {
    namespace configuration {
        /**
         * @brief Interface for any component that can be configured in the system.
         */
        class Configurable {
        public:
            /**
             * @brief Default destructor.
             */
            virtual ~Configurable() = default;

            /**
             * @brief
             * Acquires the component configurations from a given configurations map.
             *
             * @param[in] apConfigurationMap
             * The configurations map to be used.
             */
            virtual void AcquireConfiguration() = 0;
        };
    } //namespace configurations
} //namespace operations

#endif //THOTH_CONFIGURATIONS_CONFIGURABLE_HPP
