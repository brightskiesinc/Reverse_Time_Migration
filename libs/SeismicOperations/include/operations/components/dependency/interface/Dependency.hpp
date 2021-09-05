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

#ifndef OPERATIONS_LIB_COMPONENTS_DEPENDENCY_HPP
#define OPERATIONS_LIB_COMPONENTS_DEPENDENCY_HPP

#include "operations/components/dependency/helpers/ComponentsMap.tpp"

namespace operations {
    namespace components {
        namespace dependency {

            class Dependency {
            public:
                /**
                 * @brief Destructors should be overridden to ensure correct memory management.
                 */
                virtual ~Dependency() = default;

                /**
                 * @brief Sets the Dependent Components to use later on.
                 *
                 * @param[in] apDependentComponentsMap
                 * The designated Dependent Components to run operations on.
                 *
                 * @note Only components that need Dependent Components should
                 * override this function.
                 */
                virtual void SetDependentComponents(
                        operations::helpers::ComponentsMap<DependentComponent> *apDependentComponentsMap) = 0;
            };
        }//namespace dependency
    }//namespace components
}//namespace operations

#endif //OPERATIONS_LIB_COMPONENTS_DEPENDENCY_HPP
