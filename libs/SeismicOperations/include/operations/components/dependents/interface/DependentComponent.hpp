//
// Created by zeyad-osama on 27/09/2020.
//

#ifndef OPERATIONS_LIB_COMPONENTS_DEPENDENT_COMPONENT_HPP
#define OPERATIONS_LIB_COMPONENTS_DEPENDENT_COMPONENT_HPP

#include "operations/configurations/interface/Configurable.hpp"
#include "operations/common/ComputationParameters.hpp"

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
        class DependentComponent : public configuration::Configurable {
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
            operations::configuration::ConfigurationMap *mpConfigurationMap;
        };
    }//namespace components
}//namespace operations

#endif //OPERATIONS_LIB_COMPONENTS_DEPENDENT_COMPONENT_HPP
