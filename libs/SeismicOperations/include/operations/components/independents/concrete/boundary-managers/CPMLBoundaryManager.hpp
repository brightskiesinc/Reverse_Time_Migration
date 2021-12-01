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

#ifndef OPERATIONS_LIB_COMPONENTS_BOUNDARY_MANAGERS_CPML_BOUNDARY_MANAGER_HPP
#define OPERATIONS_LIB_COMPONENTS_BOUNDARY_MANAGERS_CPML_BOUNDARY_MANAGER_HPP

#include <cmath>

#include <bs/base/memory/MemoryManager.hpp>

#include <operations/components/independents/concrete/boundary-managers/extensions/Extension.hpp>
#include <operations/components/independents/primitive/BoundaryManager.hpp>
#include <operations/components/dependency/concrete/HasNoDependents.hpp>


namespace operations {
    namespace components {

        class CPMLBoundaryManager : public BoundaryManager,
                                    public dependency::HasNoDependents {
        public:
            explicit CPMLBoundaryManager(
                    bs::base::configurations::ConfigurationMap *apConfigurationMap);

            ~CPMLBoundaryManager() override;

            void ApplyBoundary(uint kernel_id) override;

            void ExtendModel() override;

            void ReExtendModel() override;

            void AdjustModelForBackward() override;

            void SetComputationParameters(common::ComputationParameters *apParameters) override;

            void SetGridBox(dataunits::GridBox *apGridBox) override;

            void AcquireConfiguration() override;

        private:
            template<int DIRECTION_>
            void FillCPMLCoefficients();

            template<int DIRECTION_, bool OPPOSITE_, int HALF_LENGTH_>
            void CalculateFirstAuxiliary();

            template<int DIRECTION_, bool OPPOSITE_, int HALF_LENGTH_>
            void CalculateCPMLValue();

            template<int HALF_LENGTH_>
            void ApplyAllCPML();

            void InitializeVariables();

            void ResetVariables();

        private:
            common::ComputationParameters *mpParameters = nullptr;

            dataunits::GridBox *mpGridBox = nullptr;

            addons::Extension *mpExtension = nullptr;

            dataunits::FrameBuffer<float> *mpCoeffax;
            dataunits::FrameBuffer<float> *mpCoeffbx;
            dataunits::FrameBuffer<float> *mpCoeffaz;
            dataunits::FrameBuffer<float> *mpCoeffbz;
            dataunits::FrameBuffer<float> *mpCoeffay;
            dataunits::FrameBuffer<float> *mpCoeffby;

            dataunits::FrameBuffer<float> *mpAux1xup;
            dataunits::FrameBuffer<float> *mpAux1xdown;
            dataunits::FrameBuffer<float> *mpAux1zup;
            dataunits::FrameBuffer<float> *mpAux1zdown;
            dataunits::FrameBuffer<float> *mpAux1yup;
            dataunits::FrameBuffer<float> *mpAux1ydown;

            dataunits::FrameBuffer<float> *mpAux2xup;
            dataunits::FrameBuffer<float> *mpAux2xdown;
            dataunits::FrameBuffer<float> *mpAux2zup;
            dataunits::FrameBuffer<float> *mpAux2zdown;
            dataunits::FrameBuffer<float> *mpAux2yup;
            dataunits::FrameBuffer<float> *mpAux2ydown;

            dataunits::FrameBuffer<float> *mpFirstCoeffx;
            dataunits::FrameBuffer<float> *mpFirstCoeffz;
            dataunits::FrameBuffer<float> *mpFirstCoeffy;
            dataunits::FrameBuffer<float> *mpSecondCoeffx;
            dataunits::FrameBuffer<float> *mpSecondCoeffz;
            dataunits::FrameBuffer<float> *mpSecondCoeffy;
            dataunits::FrameBuffer<int> *mpDistanceDim1;
            dataunits::FrameBuffer<int> *mpDistanceDim2;
            dataunits::FrameBuffer<int> *mpDistanceDim3;

            float mMaxVel;
            float mRelaxCoefficient;
            float mShiftRatio;
            float mReflectCoefficient;
            bool mUseTopLayer;
        };
    }//namespace components
}//namespace operations

#endif // OPERATIONS_LIB_COMPONENTS_BOUNDARY_MANAGERS_CPML_BOUNDARY_MANAGER_HPP
