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

#ifndef BS_BASE_CONFIGURATIONS_CONFIGURATION_MAP_HPP
#define BS_BASE_CONFIGURATIONS_CONFIGURATION_MAP_HPP

#include <string>
#include <sstream>
#include <vector>

namespace bs {
    namespace base {
        namespace configurations {
            /**
             * @brief A configuration map containing a subset of properties.
             * It is organized as subsections, with each subsection containing a list of properties.
             */
            class ConfigurationMap {
            public:
                /**
                 * @brief Default constructor.
                 */
                ConfigurationMap() = default;

                /**
                 * @brief Default destructor.
                 */
                virtual ~ConfigurationMap() = default;

                /**
                 * @brief Getter for the value of the property.
                 * it is up to the user to give the correct type for the conversion.
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
                 * T stored under this key, or the default value.
                 */
                virtual float
                GetValue(const std::string &aSectionKey,
                         const std::string &aPropertyKey,
                         float aDefaultValue) = 0;

                virtual uint
                GetValue(const std::string &aSectionKey,
                         const std::string &aPropertyKey,
                         int aDefaultValue) = 0;

                virtual double
                GetValue(const std::string &aSectionKey,
                         const std::string &aPropertyKey,
                         double aDefaultValue) = 0;

                virtual std::string
                GetValue(const std::string &aSectionKey,
                         const std::string &aPropertyKey,
                         std::string aDefaultValue) = 0;

                virtual bool
                GetValue(const std::string &aSectionKey,
                         const std::string &aPropertyKey,
                         bool aDefaultValue) = 0;

                virtual void
                WriteValue(const std::string &aSectionKey,
                           const std::string &aPropertyKey,
                           std::string aValue) = 0;

                virtual std::string
                GetValue(const std::string &aPropertyKey,
                         const std::string &aDefaultValue) = 0;

                /**
                 * @brief Check if a section entry exists in the object.
                 *
                 * @param[in] aSectionKey
                 * The section key to check.
                 *
                 * @return True if the section exists.
                 */
                virtual bool
                Contains(const std::string &aSectionKey) = 0;

                /**
                 * @brief Check if map contains the given key
                 * @param[in] aSectionKey
                 * @return
                 */
                virtual bool
                HasKey(const std::string &aSectionKey) = 0;

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
                virtual bool
                Contains(const std::string &aSectionKey,
                         const std::string &aPropertyKey) = 0;

                /**
                 * @brief
                 * Transform the configuration map into a readable string.
                 * This string is normally a formatted JSON representation.
                 *
                 * @return
                 * A string representation of the configuration map.
                 */
                virtual std::string
                ToString() = 0;

                /**
                 * @brief
                 * Operator [] overloading, to retrieve submap by a map key.
                 *
                 * @param
                 * aMapKey
                 *
                 * @return
                 * Submap identified by provided key.
                 */
                virtual std::vector<bs::base::configurations::ConfigurationMap *>
                GetConfigurationArray(std::string &aMapKey) = 0;


            };
        } //namespace configurations
    } //namespace base
} //namespace bs

#endif //BS_BASE_CONFIGURATIONS_CONFIGURATION_MAP_HPP
