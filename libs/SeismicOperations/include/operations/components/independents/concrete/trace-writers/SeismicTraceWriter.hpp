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

#ifndef OPERATIONS_LIB_COMPONENTS_TRACE_WRITERS_SEISMIC_TRACE_WRITER_HPP
#define OPERATIONS_LIB_COMPONENTS_TRACE_WRITERS_SEISMIC_TRACE_WRITER_HPP

#include <operations/components/independents/primitive/TraceWriter.hpp>
#include <operations/components/dependency/concrete/HasNoDependents.hpp>
#include <bs/io/api/cpp/BSIO.hpp>

namespace operations {
    namespace components {

        class SeismicTraceWriter : public TraceWriter, public dependency::HasNoDependents {
        public:
            explicit SeismicTraceWriter(bs::base::configurations::ConfigurationMap *apConfigurationMap);

            ~SeismicTraceWriter() override;

            void StartRecordingInstance(
                    operations::dataunits::TracesHolder &aTracesHolder) override;

            void RecordTrace(uint time_step) override;

            void FinishRecordingInstance(uint shot_id) override;

            void Finalize() override;

            void SetComputationParameters(common::ComputationParameters *apParameters) override;

            void SetGridBox(dataunits::GridBox *apGridBox) override;

            void AcquireConfiguration() override;

        private:
            common::ComputationParameters *mpParameters = nullptr;

            dataunits::GridBox *mpGridBox = nullptr;

            uint mSampleNumber;

            uint mTraceNumber;

            float mTraceSampling;

            dataunits::FrameBuffer<float> mpDTraces;

            dataunits::FrameBuffer<uint> mpDPositionsY;

            dataunits::FrameBuffer<uint> mpDPositionsX;

            bs::io::streams::Writer *mpSeismicWriter = nullptr;
        };
    }//namespace components
}//namespace operations

#endif // OPERATIONS_LIB_COMPONENTS_TRACE_WRITERS_SEISMIC_TRACE_WRITER_HPP
