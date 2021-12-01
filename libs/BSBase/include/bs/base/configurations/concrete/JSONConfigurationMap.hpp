/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of BS Base Package.
 *
 * BS Base Package is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BS Base Package is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BS_BASE_CONFIGURATIONS_JSON_CONFIGURATION_MAP_HPP
#define BS_BASE_CONFIGURATIONS_JSON_CONFIGURATION_MAP_HPP

#include <prerequisites/libraries/nlohmann/json.hpp>

#include <bs/base/configurations/interface/ConfigurationMap.hpp>

namespace bs {
    namespace base {
        namespace configurations {
            /**
             * @brief
             * A JSON implementation of the configuration map interface.
             *
             * @tparam
             * The primitive type used by the optimizer.
             */
            class JSONConfigurationMap : public ConfigurationMap {
            public:
                /**
                 * @brief Copy constructor from a nlohmann::json to be able to
                 * handle getting a JSON directly.
                 *
                 * @param[in] aJson
                 * The json given as task information.
                 */
                explicit JSONConfigurationMap(nlohmann::json aJson);

                /**
                 * @brief Default destructor.
                 */
                ~JSONConfigurationMap() override = default;

                /**
                 * @brief Getter for the value of the property.
                 * Same getter for different types of values.
                 * The type is deduced from the aDefaultValue parameter,
                 * If the key combination doesn't exist or has a null value,
                 * the given default value is returned.
                 *
                 * @param[in] aSectionKey
                 * The section key.
                 *
                 * @param[in] aPropertyKey
                 * The property key.
                 *
                 * @param[in] aDefaultValue
                 * The default value used if key doesn't not exist or is a null object.
                 *
                 * @return
                 * value stored under this key, or the default value.
                 */

                float
                GetValue(const std::string &aSectionKey,
                         const std::string &aPropertyKey,
                         float aDefaultValue) override;

                uint GetValue(const std::string &aSectionKey,
                              const std::string &aPropertyKey,
                              int aDefaultValue) override;

                double
                GetValue(const std::string &aSectionKey,
                         const std::string &aPropertyKey,
                         double aDefaultValue) override;

                std::string
                GetValue(const std::string &aSectionKey,
                         const std::string &aPropertyKey,
                         std::string aDefaultValue) override;

                bool
                GetValue(const std::string &aSectionKey,
                         const std::string &aPropertyKey,
                         bool aDefaultValue) override;


                void
                WriteValue(const std::string &aSectionKey,
                           const std::string &aPropertyKey,
                           std::string aValue) override;


                /**
                 * @brief Check if a section entry exists in the object.
                 *
                 * @param[in] aSectionKey
                 * The section key to check.
                 *
                 * @return True if the section exists.
                 */
                bool
                Contains(const std::string &aSectionKey) override;

                /**
                 * @brief Check if a property entry exists in a specific section.
                 *
                 * @param[in] aSectionKey
                 * The section key to check.
                 *
                 * @param[in] aPropertyKey
                 * The property key to check
                 *
                 * @return True if the property in that section exists.
                 */
                bool
                Contains(const std::string &aSectionKey,
                         const std::string &aPropertyKey) override;


                size_t
                Size();

                /**
                 * @brief Check if map contains the given key
                 * @param[in] aSectionKey
                 * @return
                 */
                bool
                HasKey(const std::string &aSectionKey) override;

                std::string
                GetValue(const std::string &aPropertyKey,
                         const std::string &aDefaultValue) override;

                std::string
                ToString() override;

                std::string GetKeyValue(const std::string &aPropertyKey,
                                        const std::string &aDefaultValue) override;

                /**
                 * @brief
                 * This function retrieves the submaps residing in an array in
                 * the configuration map.
                 *
                 * @param
                 * aSectionKey
                 *
                 * @return
                 * Submaps identified by provided key.
                 */
                std::vector<bs::base::configurations::ConfigurationMap *>
                GetConfigurationArray(std::string &aPropertyKey,
                                      std::string &aSectionKey) override;

                /**
                 * @brief
                 * This function retrieves the submaps residing in an array in
                 * the configuration map.
                 *
                 * @param
                 * aPropertyKey, aSectionKey
                 *
                 * @return
                 * Submaps identified by provided keys.
                 */
                std::vector<bs::base::configurations::ConfigurationMap *>
                GetConfigurationArray(std::string &aSectionKey) override;

            private:
                /// The json object used internally.
                nlohmann::json mJson;

            };
        } //namespace configurations
    } //namespace base
} //namespace bs

#endif //BS_BASE_CONFIGURATIONS_JSON_CONFIGURATION_MAP_HPP
