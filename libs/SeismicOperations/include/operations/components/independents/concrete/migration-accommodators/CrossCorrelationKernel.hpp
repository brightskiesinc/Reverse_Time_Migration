//
// Created by mirna-moawad on 10/30/19.
//

#ifndef OPERATIONS_LIB_COMPONENTS_MIGRATION_ACCOMMODATORS_CROSS_CORRELATION_KERNEL_HPP
#define OPERATIONS_LIB_COMPONENTS_MIGRATION_ACCOMMODATORS_CROSS_CORRELATION_KERNEL_HPP

#include <operations/components/independents/primitive/MigrationAccommodator.hpp>
#include <operations/components/dependency/concrete/HasNoDependents.hpp>

namespace operations {
    namespace components {

        class CrossCorrelationKernel : public MigrationAccommodator,
                                       public dependency::HasNoDependents {
        public:
            explicit CrossCorrelationKernel(operations::configuration::ConfigurationMap *apConfigurationMap);

            ~CrossCorrelationKernel() override;

            void SetComputationParameters(common::ComputationParameters *apParameters) override;

            void SetGridBox(dataunits::GridBox *apGridBox) override;

            void SetCompensation(COMPENSATION_TYPE aCOMPENSATION_TYPE) override;

            void Stack() override;

            void Correlate(dataunits::DataUnit *apDataUnit) override;

            void ResetShotCorrelation() override;

            dataunits::FrameBuffer<float> *GetShotCorrelation() override;

            dataunits::FrameBuffer<float> *GetStackedShotCorrelation() override;

            dataunits::MigrationData *GetMigrationData() override;

            void AcquireConfiguration() override;

        private:
            void InitializeInternalElements();

            template<bool _IS_2D, COMPENSATION_TYPE _COMPENSATION_TYPE>
            void Correlation(dataunits::GridBox *apGridBox);

        private:
            common::ComputationParameters *mpParameters = nullptr;

            dataunits::GridBox *mpGridBox = nullptr;

            COMPENSATION_TYPE mCompensationType;

            dataunits::FrameBuffer<float> *mpShotCorrelation = nullptr;
            dataunits::FrameBuffer<float> *mpSourceIllumination = nullptr;
            dataunits::FrameBuffer<float> *mpReceiverIllumination = nullptr;

            dataunits::FrameBuffer<float> *mpTotalCorrelation = nullptr;
            dataunits::FrameBuffer<float> *mpTotalSourceIllumination = nullptr;
            dataunits::FrameBuffer<float> *mpTotalReceiverIllumination = nullptr;
        };
    }//namespace components
}//namespace operations

#endif // OPERATIONS_LIB_COMPONENTS_MIGRATION_ACCOMMODATORS_CROSS_CORRELATION_KERNEL_HPP
