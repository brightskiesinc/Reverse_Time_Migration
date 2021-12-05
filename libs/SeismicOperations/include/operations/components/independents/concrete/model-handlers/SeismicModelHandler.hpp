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

#ifndef OPERATIONS_LIB_COMPONENTS_MODEL_HANDLER_SEISMIC_MODEL_HANDLER_HPP
#define OPERATIONS_LIB_COMPONENTS_MODEL_HANDLER_SEISMIC_MODEL_HANDLER_HPP

#include <map>

#include <bs/base/memory/MemoryManager.hpp>

#include <operations/components/dependents/concrete/memory-handlers/WaveFieldsMemoryHandler.hpp>
#include <operations/components/independents/primitive/ModelHandler.hpp>
#include <operations/components/dependency/concrete/HasDependents.hpp>

namespace operations {
    namespace components {

        class SeismicModelHandler : public ModelHandler, public dependency::HasDependents {

        public:
            explicit SeismicModelHandler(bs::base::configurations::ConfigurationMap *apConfigurationMap);

            ~SeismicModelHandler() override;

            void SetComputationParameters(common::ComputationParameters *apParameters) override;

            void SetGridBox(dataunits::GridBox *apGridBox) override;

            void SetDependentComponents(
                    operations::helpers::ComponentsMap<DependentComponent> *apDependentComponentsMap) override;

            dataunits::GridBox *ReadModel(std::map<std::string, std::string> file_names) override;

            void SetupWindow() override;

            void AcquireConfiguration() override;

            void PostProcessMigration(dataunits::MigrationData *apMigrationData) override;

        private:
            float GetSuitableDT(float *coefficients, std::map<std::string, float> maximums,
                                int half_length, float dt_relax);

            void Initialize(std::map<std::string, std::string> file_names);

            void RegisterWaveFields(uint nx, uint ny, uint nz);

            void RegisterParameters(uint nx, uint ny, uint nz);

            void SetupPadding();

            void AllocateWaveFields();

            void AllocateParameters();

        private:
            common::ComputationParameters *mpParameters = nullptr;

            dataunits::GridBox *mpGridBox = nullptr;

            WaveFieldsMemoryHandler *mpWaveFieldsMemoryHandler = nullptr;

            std::vector<std::pair<dataunits::GridBox::Key, std::string>> PARAMS_NAMES;

            std::vector<dataunits::GridBox::Key> WAVE_FIELDS_NAMES;

            std::string mReaderType;

            float mDepthSamplingScaler;
        };
    }//namespace components
}//namespace operations

#endif //OPERATIONS_LIB_COMPONENTS_MODEL_HANDLER_SEISMIC_MODEL_HANDLER_HPP
