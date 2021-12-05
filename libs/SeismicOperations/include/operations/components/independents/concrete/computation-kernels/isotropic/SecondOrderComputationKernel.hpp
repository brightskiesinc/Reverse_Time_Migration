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

#ifndef OPERATIONS_LIB_COMPONENTS_COMPUTATION_KERNELS_SECOND_ORDER_COMPUTATION_KERNEL_HPP
#define OPERATIONS_LIB_COMPONENTS_COMPUTATION_KERNELS_SECOND_ORDER_COMPUTATION_KERNEL_HPP

#include <operations/components/independents/primitive/ComputationKernel.hpp>
#include <operations/components/dependency/concrete/HasNoDependents.hpp>

namespace operations {
    namespace components {

        class SecondOrderComputationKernel : public ComputationKernel,

                                             public dependency::HasNoDependents {
        public:
            explicit SecondOrderComputationKernel(bs::base::configurations::ConfigurationMap *apConfigurationMap);

            SecondOrderComputationKernel(const SecondOrderComputationKernel &);

            ~SecondOrderComputationKernel() override;

            ComputationKernel *Clone() override;

            void SetComputationParameters(common::ComputationParameters *apParameters) override;

            void SetGridBox(dataunits::GridBox *apGridBox) override;

            void Step() override;

            MemoryHandler *GetMemoryHandler() override;

            void AcquireConfiguration() override;

            void PreprocessModel() override;

        private:
            template<KERNEL_MODE KERNEL_MODE_>
            void Compute();

            template<KERNEL_MODE KERNEL_MODE_, bool IS_2D_>
            void Compute();

            template<KERNEL_MODE KERNEL_MODE_, bool IS_2D_, HALF_LENGTH HALF_LENGTH_>
            void Compute();

            void InitializeVariables();

        private:
            common::ComputationParameters *mpParameters = nullptr;

            dataunits::GridBox *mpGridBox = nullptr;

            dataunits::FrameBuffer<float> *mpCoeffX = nullptr;
            dataunits::FrameBuffer<float> *mpCoeffY = nullptr;
            dataunits::FrameBuffer<float> *mpCoeffZ = nullptr;

            dataunits::FrameBuffer<int> *mpFrontalIdx = nullptr;
            dataunits::FrameBuffer<int> *mpVerticalIdx = nullptr;

            float mCoeffXYZ;
        };
    }//namespace components
}//namespace operations

#endif // OPERATIONS_LIB_COMPONENTS_COMPUTATION_KERNELS_SECOND_ORDER_COMPUTATION_KERNEL_HPP
