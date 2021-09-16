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

#ifndef OPERATIONS_LIB_COMPONENTS_HAS_DEPENDENTS_HPP
#define OPERATIONS_LIB_COMPONENTS_HAS_DEPENDENTS_HPP

#include <operations/components/dependency/interface/Dependency.hpp>
#include <operations/components/dependency/helpers/ComponentsMap.tpp>
#include <operations/components/dependents/interface/DependentComponent.hpp>
#include <operations/common/ComputationParameters.hpp>
#include <bs/base/logger/concrete/LoggerSystem.hpp>

#include <cstdlib>

namespace operations {
    namespace components {
        namespace dependency {

            /**
             * @brief Has Dependents Component interface. All Independent Component
             * in the Seismic Framework needs to extend it in case they need a
             * dependent component.
             *
             * @relatedalso class HasNoDependents
             *
             * @note
             */
            class HasDependents : virtual public Dependency {
            public:
                /**
                 * @brief Destructors should be overridden to ensure correct memory management.
                 */
                virtual ~HasDependents() = default;

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
                        operations::helpers::ComponentsMap<DependentComponent> *apDependentComponentsMap) {
                    bs::base::logger::LoggerSystem *Logger = bs::base::logger::LoggerSystem::GetInstance();
                    this->mpDependentComponentsMap = apDependentComponentsMap;
                    if (this->mpDependentComponentsMap == nullptr) {
                        Logger->Error() << "No Dependent Components Map provided... "
                                        << "Terminating..." << '\n';
                        exit(EXIT_FAILURE);
                    }
                }

                /**
                 * @brief Dependent Components Map getter.
                 * @return[out] DependentComponentsMap *
                 */
                inline operations::helpers::ComponentsMap<DependentComponent> *
                GetDependentComponentsMap() const {
                    return this->mpDependentComponentsMap;
                }

            private:
                /// Dependent Components Map
                operations::helpers::ComponentsMap<DependentComponent> *mpDependentComponentsMap;
            };
        }//namespace dependency
    }//namespace components
}//namespace operations

#endif //OPERATIONS_LIB_COMPONENTS_HAS_DEPENDENTS_HPP
