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

#ifndef OPERATIONS_LIB_COMPONENTS_TRACE_MANAGERS_SEISMIC_TRACE_MANAGER_HPP
#define OPERATIONS_LIB_COMPONENTS_TRACE_MANAGERS_SEISMIC_TRACE_MANAGER_HPP

#include <operations/components/independents/primitive/TraceManager.hpp>
#include <operations/components/dependency/concrete/HasNoDependents.hpp>
#include <operations/data-units/concrete/holders/FrameBuffer.hpp>

#include <bs/io/api/cpp/BSIO.hpp>

#include <fstream>
#include <unordered_map>

namespace operations {
    namespace components {

        class SeismicTraceManager : public TraceManager,
                                    public dependency::HasNoDependents {
        public:
            SeismicTraceManager(bs::base::configurations::ConfigurationMap *apConfigurationMap);

            ~SeismicTraceManager() override;

            void SetComputationParameters(common::ComputationParameters *apParameters) override;

            void SetGridBox(dataunits::GridBox *apGridBox) override;

            void ReadShot(std::vector<std::string> file_names, uint shot_number,
                          std::string sort_key) override;

            void PreprocessShot() override;

            void ApplyTraces(int time_step) override;

            void ApplyIsotropicField() override;

            void RevertIsotropicField() override;

            dataunits::TracesHolder *GetTracesHolder() override;

            Point3D *GetSourcePoint() override;

            std::vector<uint> GetWorkingShots(std::vector<std::string> file_names,
                                              uint min_shot, uint max_shot, std::string type) override;

            void AcquireConfiguration() override;

        private:
            common::ComputationParameters *mpParameters = nullptr;

            dataunits::GridBox *mpGridBox = nullptr;

            Point3D mpSourcePoint;

            bs::io::streams::Reader *mpSeismicReader = nullptr;

            INTERPOLATION mInterpolation;

            dataunits::TracesHolder *mpTracesHolder = nullptr;

            dataunits::FrameBuffer<float> mpDTraces;

            dataunits::FrameBuffer<uint> mpDPositionsY;

            dataunits::FrameBuffer<uint> mpDPositionsX;

            float mTotalTime;

            int mShotStride;
        };
    }//namespace components
}//namespace operations

#endif // OPERATIONS_LIB_COMPONENTS_TRACE_MANAGERS_SEISMIC_TRACE_MANAGER_HPP
