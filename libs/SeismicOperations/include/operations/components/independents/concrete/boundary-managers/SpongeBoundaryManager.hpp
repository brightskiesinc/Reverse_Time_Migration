//
// Created by mirna-moawad on 11/21/19.
//

#ifndef OPERATIONS_LIB_COMPONENTS_BOUNDARY_MANAGERS_SPONGE_BOUNDARY_MANAGER_HPP
#define OPERATIONS_LIB_COMPONENTS_BOUNDARY_MANAGERS_SPONGE_BOUNDARY_MANAGER_HPP

#include <operations/components/independents/concrete/boundary-managers/extensions/Extension.hpp>
#include <operations/components/independents/primitive/BoundaryManager.hpp>
#include <operations/components/dependency/concrete/HasNoDependents.hpp>

#include <vector>

namespace operations {
    namespace components {

        class SpongeBoundaryManager : public BoundaryManager,
                                      public dependency::HasNoDependents {
        public:
            explicit SpongeBoundaryManager(operations::configuration::ConfigurationMap *apConfigurationMap);

            ~SpongeBoundaryManager() override;

            void ApplyBoundary(uint kernel_id) override;

            void ExtendModel() override;

            void ReExtendModel() override;

            void SetComputationParameters(common::ComputationParameters *apParameters) override;

            void SetGridBox(dataunits::GridBox *apGridBox) override;

            void AdjustModelForBackward() override;

            void AcquireConfiguration() override;

        private:
            void InitializeExtensions();

            float Calculation(int index);

            void ApplyBoundaryOnField(float *next);

        private:
            dataunits::GridBox *mpGridBox = nullptr;

            common::ComputationParameters *mpParameters = nullptr;

            std::vector<addons::Extension *> mvExtensions;

            dataunits::FrameBuffer<float> *mpSpongeCoefficients;

            bool mUseTopLayer;
        };
    }//namespace components
}//namespace operations

#endif // OPERATIONS_LIB_COMPONENTS_BOUNDARY_MANAGERS_SPONGE_BOUNDARY_MANAGER_HPP
