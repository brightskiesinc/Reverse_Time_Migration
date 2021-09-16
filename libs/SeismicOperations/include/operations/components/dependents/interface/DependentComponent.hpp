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

#ifndef OPERATIONS_LIB_COMPONENTS_DEPENDENT_COMPONENT_HPP
#define OPERATIONS_LIB_COMPONENTS_DEPENDENT_COMPONENT_HPP

#include <operations/common/ComputationParameters.hpp>

#include <bs/base/configurations/interface/Configurable.hpp>

namespace operations {
    namespace components {
        /**
         * @brief Dependent Component interface. All Dependent Component
         * in the Seismic Framework needs to extend it.
         *
         * @note Each engine comes with it's own Timer and Logger. Logger
         * Channel should be initialized at each concrete implementation
         * and should be destructed at each destructor.
         */
        class DependentComponent : public bs::base::configurations::Configurable {
        public:
            /**
             * @brief Destructors should be overridden to ensure correct memory management.
             */
            virtual ~DependentComponent() = default;

            /**
             * @brief Sets the computation parameters to be used for the component.
             *
             * @param[in] apParameters
             * Parameters of the simulation independent from the grid.
             */
            virtual void SetComputationParameters(common::ComputationParameters *apParameters) = 0;

        protected:
            /// Configurations Map
            bs::base::configurations::ConfigurationMap *mpConfigurationMap;
        };
    }//namespace components
}//namespace operations

#endif //OPERATIONS_LIB_COMPONENTS_DEPENDENT_COMPONENT_HPP
