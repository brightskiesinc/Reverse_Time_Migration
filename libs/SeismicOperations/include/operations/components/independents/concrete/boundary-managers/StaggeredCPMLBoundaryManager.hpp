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

#ifndef OPERATIONS_LIB_COMPONENTS_BOUNDARY_MANAGERS_STAGGERED_CPML_BOUNDARY_MANAGER_HPP
#define OPERATIONS_LIB_COMPONENTS_BOUNDARY_MANAGERS_STAGGERED_CPML_BOUNDARY_MANAGER_HPP

#include <operations/components/independents/concrete/boundary-managers/extensions/Extension.hpp>
#include <operations/components/independents/primitive/BoundaryManager.hpp>
#include <operations/components/dependency/concrete/HasNoDependents.hpp>

#include <bs/base/memory/MemoryManager.hpp>

#include <vector>

namespace operations {
    namespace components {

        class StaggeredCPMLBoundaryManager : public BoundaryManager,
                                             public dependency::HasNoDependents {
        public:
            explicit StaggeredCPMLBoundaryManager(bs::base::configurations::ConfigurationMap *apConfigurationMap);

            ~StaggeredCPMLBoundaryManager() override;

            void ApplyBoundary(uint kernel_id) override;

            void ExtendModel() override;

            void ReExtendModel() override;

            void SetComputationParameters(common::ComputationParameters *apParameters) override;

            void SetGridBox(dataunits::GridBox *apGridBox) override;

            void AdjustModelForBackward() override;

            void AcquireConfiguration() override;

        private:
            void InitializeExtensions();

            void FillCPMLCoefficients(float *coeff_a, float *coeff_b, int boundary_length,
                                      float dh, float dt, float max_vel, float shift_ratio,
                                      float reflect_coeff, float relax_cp);

            void ZeroAuxiliaryVariables();

            template<bool ADJOINT_, int DIRECTION_, bool OPPOSITE_, int HALF_LENGTH_>
            void CalculateVelocityFirstAuxiliary();

            template<bool ADJOINT_, int DIRECTION_, bool OPPOSITE_, int HALF_LENGTH_>
            void CalculateVelocityCPMLValue();

            template<bool ADJOINT_, int DIRECTION_, bool OPPOSITE_, int HALF_LENGTH_>
            void CalculatePressureFirstAuxiliary();

            template<bool ADJOINT_, int DIRECTION_, bool OPPOSITE_, int HALF_LENGTH_>
            void CalculatePressureCPMLValue();

            template<bool ADJOINT_, int HALF_LENGTH_>
            void ApplyVelocityCPML();

            template<bool ADJOINT_, int HALF_LENGTH_>
            void ApplyPressureCPML();

            template<bool ADJOINT_>
            void ApplyVelocityCPML();

            template<bool ADJOINT_>
            void ApplyPressureCPML();

        private:
            common::ComputationParameters *mpParameters = nullptr;

            dataunits::GridBox *mpGridBox = nullptr;

            std::vector<addons::Extension *> mvExtensions;

            bool mUseTopLayer;

            float mMaxVelocity;

            float mReflectCoefficient = 0.1;
            float mShiftRatio = 0.1;
            float mRelaxCoefficient = 1;

            dataunits::FrameBuffer<float> *small_a_x = nullptr;
            dataunits::FrameBuffer<float> *small_a_y = nullptr;
            dataunits::FrameBuffer<float> *small_a_z = nullptr;
            dataunits::FrameBuffer<float> *small_b_x = nullptr;
            dataunits::FrameBuffer<float> *small_b_y = nullptr;
            dataunits::FrameBuffer<float> *small_b_z = nullptr;

            dataunits::FrameBuffer<float> *auxiliary_vel_x_left = nullptr;
            dataunits::FrameBuffer<float> *auxiliary_vel_x_right = nullptr;

            dataunits::FrameBuffer<float> *auxiliary_vel_y_up = nullptr;
            dataunits::FrameBuffer<float> *auxiliary_vel_y_down = nullptr;

            dataunits::FrameBuffer<float> *auxiliary_vel_z_up = nullptr;
            dataunits::FrameBuffer<float> *auxiliary_vel_z_down = nullptr;

            dataunits::FrameBuffer<float> *auxiliary_ptr_x_left = nullptr;
            dataunits::FrameBuffer<float> *auxiliary_ptr_x_right = nullptr;

            dataunits::FrameBuffer<float> *auxiliary_ptr_y_up = nullptr;
            dataunits::FrameBuffer<float> *auxiliary_ptr_y_down = nullptr;

            dataunits::FrameBuffer<float> *auxiliary_ptr_z_up = nullptr;
            dataunits::FrameBuffer<float> *auxiliary_ptr_z_down = nullptr;
        };
    }//namespace components
}//namespace operations

#endif //OPERATIONS_LIB_COMPONENTS_BOUNDARY_MANAGERS_STAGGERED_CPML_BOUNDARY_MANAGER_HPP
