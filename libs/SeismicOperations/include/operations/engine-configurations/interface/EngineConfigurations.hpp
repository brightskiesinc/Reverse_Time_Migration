//
// Created by zeyad-osama on 20/09/2020.
//

#ifndef OPERATIONS_LIB_ENGINE_CONFIGURATIONS_ENGINE_CONFIGURATION_HPP
#define OPERATIONS_LIB_ENGINE_CONFIGURATIONS_ENGINE_CONFIGURATION_HPP

#include "operations/components/independents/interface/Component.hpp"
#include "operations/components/dependents/interface/DependentComponent.hpp"
#include "operations/components/dependency/helpers/ComponentsMap.tpp"

namespace operations {
    namespace configuration {
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
                delete mpDependentComponentsMap;
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
