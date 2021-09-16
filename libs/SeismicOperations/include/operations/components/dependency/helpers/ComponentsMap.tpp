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

#ifndef OPERATIONS_LIB_COMPONENTS_DEPENDENT_COMPONENTS_MAP_TPP
#define OPERATIONS_LIB_COMPONENTS_DEPENDENT_COMPONENTS_MAP_TPP

#include <operations/components/dependents/interface/DependentComponent.hpp>
#include <bs/base/exceptions/Exceptions.hpp>

#include <vector>
#include <map>

namespace operations {
    namespace helpers {

        template<typename T>
        class ComponentsMap {
        public:
            ~ComponentsMap() = default;

            void Set(uint key, T *apDependentComponent) {
                this->mComponentsMap[key] = apDependentComponent;
            }

            T *Get(uint key) {
                if (this->mComponentsMap.find(key) ==
                    this->mComponentsMap.end()) {
                    throw bs::base::exceptions::NO_KEY_FOUND_EXCEPTION();
                }
                return this->mComponentsMap[key];
            }

            std::vector<T *> ExtractValues() {
                std::vector<T *> values;
                for (auto const &dependent_components : this->mComponentsMap) {
                    values.push_back(dependent_components.second);
                }
                return values;
            }

        private:
            std::map<uint, T *> mComponentsMap;
        };

/*
 * Indices.
 */
#define MEMORY_HANDLER 0

#define MODEL_HANDLER 1
#define COMPUTATION_KERNEL 2
#define MIGRATION_ACCOMMODATOR 3
#define BOUNDARY_MANAGER 4
#define SOURCE_INJECTOR 5
#define TRACE_MANAGER 6
#define RAY_TRACER 7
#define RESIDUAL_MANAGER 8
#define STOPPAGE_CRITERIA 9
#define MODEL_UPDATER 10
#define FORWARD_COLLECTOR 11
#define TRACE_WRITER 12
#define MODELLING_CONFIG_PARSER 13

    }//namespace helpers
}//namespace operations

#endif //OPERATIONS_LIB_COMPONENTS_DEPENDENT_COMPONENTS_MAP_TPP
