//
// Created by zeyad-osama on 02/11/2020.
//

#ifndef THOTH_CONFIGURATION_LIST_HPP
#define THOTH_CONFIGURATION_LIST_HPP

#include <thoth/properties/interface/Property.hpp>

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace thoth {
    namespace properties {
        /**
         * @brief
         */
        class PropertiesConfiguration {
        public:
            typedef std::vector<Property> ConfigurationList;

        public:
            /**
             * @brief Destructor
             */
            ~PropertiesConfiguration() = default;

            /**
             * @brief
             */
            static void Register(ConfigurationList &apConfigurationList, const Property &aProperty);
        };

    } //streams
} //thoth

#endif //THOTH_CONFIGURATION_LIST_HPP
