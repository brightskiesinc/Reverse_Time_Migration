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

#ifndef OPERATIONS_LIB_HELPERS_CALLBACKS_NORM_WRITER_H
#define OPERATIONS_LIB_HELPERS_CALLBACKS_NORM_WRITER_H

#include <operations/helpers/callbacks/interface/Callback.hpp>

#include <fstream>
#include <string>

namespace operations {
    namespace helpers {
        namespace callbacks {

            class NormWriter : public Callback {
            public:
                NormWriter(uint aShowEach,
                           bool aWriteForward,
                           bool aWriteBackward,
                           bool aWriteReverse,
                           const std::string &aWritePath);

                ~NormWriter();

                void
                BeforeInitialization(common::ComputationParameters *apParameters) override;

                void
                AfterInitialization(dataunits::GridBox *apGridBox) override;

                void
                BeforeShotPreprocessing(dataunits::TracesHolder *apTraces) override;

                void
                AfterShotPreprocessing(dataunits::TracesHolder *apTraces) override;

                void
                BeforeForwardPropagation(dataunits::GridBox *apGridBox) override;

                void
                AfterForwardStep(dataunits::GridBox *apGridBox, int aTimeStep) override;

                void
                BeforeBackwardPropagation(dataunits::GridBox *apGridBox) override;

                void
                AfterBackwardStep(dataunits::GridBox *apGridBox, int aTimeStep) override;

                void
                AfterFetchStep(dataunits::GridBox *apGridBox, int aTimeStep) override;

                void
                BeforeShotStacking(dataunits::GridBox *apGridBox,
                                   dataunits::FrameBuffer<float> *apShotCorrelation) override;

                void
                AfterShotStacking(dataunits::GridBox *apGridBox,
                                  dataunits::FrameBuffer<float> *apStackedShotCorrelation) override;

                void
                AfterMigration(dataunits::GridBox *apGridBox,
                               dataunits::FrameBuffer<float> *apStackedShotCorrelation) override;


            public:
                std::string GetExtension();

            public:
                static float Solve(const float *apMatrix, uint nx, uint nz, uint ny);

            private:
                uint show_each;
                bool write_forward;
                bool write_backward;
                bool write_reverse;
                std::string write_path;

                std::ofstream *forward_norm_stream;
                std::ofstream *reverse_norm_stream;
                std::ofstream *backward_norm_stream;
            };
        } //namespace callbacks
    } //namespace operations
} //namespace operations

#endif // OPERATIONS_LIB_HELPERS_CALLBACKS_NORM_WRITER_H
