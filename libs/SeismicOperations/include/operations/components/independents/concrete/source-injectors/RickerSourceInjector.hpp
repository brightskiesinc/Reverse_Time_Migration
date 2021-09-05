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
#ifndef OPERATIONS_LIB_COMPONENTS_SOURCE_INJECTORS_RICKER_SOURCE_INJECTOR_HPP
#define OPERATIONS_LIB_COMPONENTS_SOURCE_INJECTORS_RICKER_SOURCE_INJECTOR_HPP

#include <operations/components/independents/primitive/SourceInjector.hpp>
#include <operations/components/dependency/concrete/HasNoDependents.hpp>

namespace operations {
    namespace components {

        class RickerSourceInjector : public SourceInjector, public dependency::HasNoDependents {
        public:
            explicit RickerSourceInjector(bs::base::configurations::ConfigurationMap *apConfigurationMap);

            ~RickerSourceInjector() override;

            void ApplySource(int time_step) override;

            void ApplyIsotropicField() override;

            void RevertIsotropicField() override;

            int GetCutOffTimeStep() override;

            int GetPrePropagationNT() override;

            float GetMaxFrequency() override;

            void SetComputationParameters(common::ComputationParameters *apParameters) override;

            void SetGridBox(dataunits::GridBox *apGridBox) override;

            void SetSourcePoint(Point3D *apSourcePoint) override;

            void AcquireConfiguration() override;

        private:
            uint GetInjectionLocation();

        private:
            common::ComputationParameters *mpParameters = nullptr;

            dataunits::GridBox *mpGridBox = nullptr;

            Point3D *mpSourcePoint = nullptr;

            float mMaxFrequencyAmplitudePercentage;

            float mMaxFrequency;
        };
    }//namespace components
}//namespace operations

#endif // OPERATIONS_LIB_COMPONENTS_SOURCE_INJECTORS_RICKER_SOURCE_INJECTOR_HPP
