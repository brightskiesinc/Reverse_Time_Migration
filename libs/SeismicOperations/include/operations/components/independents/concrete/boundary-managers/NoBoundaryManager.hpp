//
// Created by amr-nasr on 21/10/2019.
//

#ifndef OPERATIONS_LIB_COMPONENTS_BOUNDARY_MANAGERS_NO_BOUNDARY_MANAGER_HPP
#define OPERATIONS_LIB_COMPONENTS_BOUNDARY_MANAGERS_NO_BOUNDARY_MANAGER_HPP

#include <operations/components/independents/concrete/boundary-managers/extensions/Extension.hpp>
#include <operations/components/independents/primitive/BoundaryManager.hpp>
#include <operations/components/dependency/concrete/HasNoDependents.hpp>

#include <vector>

namespace operations {
    namespace components {

        class NoBoundaryManager : public BoundaryManager,
                                  public dependency::HasNoDependents {
        public:
            explicit NoBoundaryManager(operations::configuration::ConfigurationMap *apConfigurationMap);

            ~NoBoundaryManager() override;

            void ApplyBoundary(uint kernel_id) override;

            void ExtendModel() override;

            void ReExtendModel() override;

            void SetComputationParameters(common::ComputationParameters *apParameters) override;

            void SetGridBox(dataunits::GridBox *apGridBox) override;

            void AdjustModelForBackward() override;

            void AcquireConfiguration() override;

        private:
            void InitializeExtensions();

        private:
            common::ComputationParameters *mpParameters = nullptr;

            dataunits::GridBox *mpGridBox = nullptr;

            std::vector<addons::Extension *> mvExtensions;
        };
    }//namespace components
}//namespace operations

#endif // OPERATIONS_LIB_COMPONENTS_BOUNDARY_MANAGERS_NO_BOUNDARY_MANAGER_HPP
