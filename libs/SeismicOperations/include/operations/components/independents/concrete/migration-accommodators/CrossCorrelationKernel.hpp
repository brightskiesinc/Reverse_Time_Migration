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

#ifndef OPERATIONS_LIB_COMPONENTS_MIGRATION_ACCOMMODATORS_CROSS_CORRELATION_KERNEL_HPP
#define OPERATIONS_LIB_COMPONENTS_MIGRATION_ACCOMMODATORS_CROSS_CORRELATION_KERNEL_HPP

#include <operations/components/independents/primitive/MigrationAccommodator.hpp>
#include <operations/components/dependency/concrete/HasNoDependents.hpp>

namespace operations {
    namespace components {

        class CrossCorrelationKernel : public MigrationAccommodator,
                                       public dependency::HasNoDependents {
        public:
            explicit CrossCorrelationKernel(bs::base::configurations::ConfigurationMap *apConfigurationMap);

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

            void SetSourcePoint(Point3D *apSourcePoint) override;

        private:
            uint CalculateDipAngleDepth(double aDipAngle, int aCurrentPositionX,
                                        int aCurrentPositionY) const;

            void InitializeInternalElements();

            template<bool _IS_2D, COMPENSATION_TYPE _COMPENSATION_TYPE>
            void Correlation(dataunits::GridBox *apGridBox);

            template<bool _IS_2D, COMPENSATION_TYPE _COMPENSATION_TYPE>
            void Stack();

        private:
            common::ComputationParameters *mpParameters = nullptr;

            dataunits::GridBox *mpGridBox = nullptr;

            Point3D mSourcePoint{};

            COMPENSATION_TYPE mCompensationType;

            dataunits::FrameBuffer<float> *mpShotCorrelation = nullptr;
            dataunits::FrameBuffer<float> *mpSourceIllumination = nullptr;
            dataunits::FrameBuffer<float> *mpReceiverIllumination = nullptr;

            dataunits::FrameBuffer<float> *mpTotalCorrelation = nullptr;
        };
    }//namespace components
}//namespace operations

#endif // OPERATIONS_LIB_COMPONENTS_MIGRATION_ACCOMMODATORS_CROSS_CORRELATION_KERNEL_HPP
