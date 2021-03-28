//
// Created by zeyad-osama on 02/11/2020.
//

#ifndef THOTH__CONFIGURATIONS_PROPERTY_HPP
#define THOTH__CONFIGURATIONS_PROPERTY_HPP

#include <string>
#include <map>

namespace thoth {
    namespace properties {
        /**
         * @brief
         */
        class Property {
        public:

            explicit Property(std::string &apPropertyKey, std::string &apPropertyValue) {
                this->mPropertyKey = apPropertyKey;
                this->mPropertyValue = apPropertyValue;
            }

            /**
             * @brief Destructor
             */
            virtual ~Property() = default;

        private:
            std::string mPropertyKey;
            std::string mPropertyValue;
        };

    } //properties
} //thoth

#endif //THOTH__CONFIGURATIONS_PROPERTY_HPP
