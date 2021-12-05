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

#ifndef OPERATIONS_LIB_HELPERS_CALLBACKS_WRITER_CALLBACK_H
#define OPERATIONS_LIB_HELPERS_CALLBACKS_WRITER_CALLBACK_H

#include <string>
#include <vector>

#include <bs/io/api/cpp/BSIO.hpp>
#include <operations/helpers/callbacks/interface/Callback.hpp>

namespace operations {
    namespace helpers {
        namespace callbacks {

            class WriterCallback : public Callback {
            public:
                WriterCallback(uint aShowEach,
                               bool aWriteParams,
                               bool aWriteForward,
                               bool aWriteBackward,
                               bool aWriteReverse,
                               bool aWriteMigration,
                               bool aWriteReExtendedParams,
                               bool aWriteSingleShotCorrelation,
                               bool aWriteEachStackedShot,
                               bool aWriteTracesRaw,
                               bool aWriteTracesPreprocessed,
                               const std::vector<std::string> &aVecParams,
                               const std::vector<std::string> &aVecReExtendedParams,
                               const std::string &aWritePath,
                               std::vector<std::string> &aTypes,
                               std::vector<std::string> &aUnderlyingConfigurations);

                ~WriterCallback();

                void
                WriteResult(uint nx, uint ny, uint ns,
                            float dx, float dy, float ds,
                            const float *data, const std::string &filename,
                            float sample_scale);

                void
                BeforeInitialization(common::ComputationParameters *apParameters) override;

                void
                AfterInitialization(dataunits::GridBox *apGridBox) override;

                void
                BeforeShotPreprocessing(dataunits::TracesHolder *traces) override;

                void
                AfterShotPreprocessing(dataunits::TracesHolder *traces) override;

                void
                BeforeForwardPropagation(dataunits::GridBox *apGridBox) override;

                void
                AfterForwardStep(dataunits::GridBox *box, int time_step) override;

                void
                BeforeBackwardPropagation(dataunits::GridBox *apGridBox) override;

                void
                AfterBackwardStep(dataunits::GridBox *apGridBox, int time_step) override;

                void
                AfterFetchStep(dataunits::GridBox *apGridBox, int time_step) override;

                void
                BeforeShotStacking(dataunits::GridBox *apGridBox,
                                   dataunits::FrameBuffer<float> *shot_correlation) override;

                void
                AfterShotStacking(dataunits::GridBox *apGridBox,
                                  dataunits::FrameBuffer<float> *stacked_shot_correlation) override;

                void
                AfterMigration(dataunits::GridBox *apGridBox,
                               dataunits::FrameBuffer<float> *stacked_shot_correlation) override;

            private:
                uint mShowEach;
                uint mShotCount;
                bool mIsWriteParams;
                bool mIsWriteForward;
                bool mIsWriteBackward;
                bool mIsWriteReverse;
                bool mIsWriteMigration;
                bool mIsWriteReExtendedParams;
                bool mIsWriteSingleShotCorrelation;
                bool mIsWriteEachStackedShot;
                bool mIsWriteTracesRaw;
                bool mIsWriteTracesPreprocessed;
                std::string mWritePath;
                std::vector<std::string> mParamsVec;
                std::vector<std::string> mReExtendedParamsVec;
                std::vector<std::string> mWriterTypes;
                std::vector<bs::io::streams::SeismicWriter *> mWriters;
            };
        } //namespace callbacks
    } //namespace operations
} //namespace operations

#endif // OPERATIONS_LIB_HELPERS_CALLBACKS_WRITER_CALLBACK_H
