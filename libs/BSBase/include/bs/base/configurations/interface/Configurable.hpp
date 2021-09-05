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

#ifndef BS_BASE_CONFIGURATIONS_CONFIGURABLE_HPP
#define BS_BASE_CONFIGURATIONS_CONFIGURABLE_HPP

#include <bs/base/configurations/interface/ConfigurationMap.hpp>

namespace bs {
    namespace base {
        namespace configurations {
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
                 * Acquires the component configuration from a given configuration map.
                 *
                 * @param[in] apConfigurationMap
                 * The configuration map to be used.
                 */
                virtual void AcquireConfiguration() = 0;
            };
        } //namespace configurations
    } //namespace base
} //namespace bs

#endif //BS_BASE_CONFIGURATIONS_CONFIGURABLE_HPP
