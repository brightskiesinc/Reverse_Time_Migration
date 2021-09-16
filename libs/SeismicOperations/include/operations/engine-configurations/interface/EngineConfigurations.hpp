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

#ifndef OPERATIONS_LIB_ENGINE_CONFIGURATIONS_ENGINE_CONFIGURATION_HPP
#define OPERATIONS_LIB_ENGINE_CONFIGURATIONS_ENGINE_CONFIGURATION_HPP

#include "operations/components/independents/interface/Component.hpp"
#include "operations/components/dependents/interface/DependentComponent.hpp"
#include "operations/components/dependency/helpers/ComponentsMap.tpp"

namespace operations {
    namespace configurations {
/**
 * @note
 * Whether you need a destructor is NOT determined by whether you
 * use a struct or class. The deciding factor is whether the struct/class has
 * acquired resources that must be released explicitly when the life of the
 * object ends. If the answer to the question is yes, then you need to implement
 * a destructor. Otherwise, you don't need to implement it.
 */

        /**
         * @brief Class that will contain pointers to concrete
         * implementations of each component to be used in the the
         * desired framework engine.
         */
        class EngineConfigurations {
        public:
            EngineConfigurations() {
                this->mpComponentsMap = new helpers::ComponentsMap<components::Component>();
                this->mpDependentComponentsMap = new helpers::ComponentsMap<components::DependentComponent>();
            }

            virtual ~EngineConfigurations() {
                delete this->mpDependentComponentsMap;
                delete this->mpComponentsMap;
            }

            inline virtual helpers::ComponentsMap<components::Component> *GetComponents() {
                return this->mpComponentsMap;
            }

            inline virtual helpers::ComponentsMap<components::DependentComponent> *GetDependentComponents() {
                return this->mpDependentComponentsMap;
            }

        protected:
            helpers::ComponentsMap<components::Component> *mpComponentsMap;

            helpers::ComponentsMap<components::DependentComponent> *mpDependentComponentsMap;
        };
    } //namespace configuration
} //namespace operations

#endif //OPERATIONS_LIB_ENGINE_CONFIGURATIONS_ENGINE_CONFIGURATION_HPP
