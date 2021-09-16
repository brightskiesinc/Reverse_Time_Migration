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

#ifndef OPERATIONS_LIB_CALLBACK_COLLECTION_HPP
#define OPERATIONS_LIB_CALLBACK_COLLECTION_HPP

#include <operations/helpers/callbacks/interface/Callback.hpp>

#include <vector>

namespace operations {
    namespace helpers {
        namespace callbacks {

            class CallbackCollection {
            public:
                void
                RegisterCallback(Callback *apCallback);

                void
                BeforeInitialization(common::ComputationParameters *apParameters);

                void
                AfterInitialization(dataunits::GridBox *apGridBox);

                void
                BeforeShotPreprocessing(dataunits::TracesHolder *apTraces);

                void
                AfterShotPreprocessing(dataunits::TracesHolder *apTraces);

                void
                BeforeForwardPropagation(dataunits::GridBox *apGridBox);

                void
                AfterForwardStep(dataunits::GridBox *apGridBox, int time_step);

                void
                BeforeBackwardPropagation(dataunits::GridBox *apGridBox);

                void
                AfterBackwardStep(dataunits::GridBox *apGridBox, int time_step);

                void
                AfterFetchStep(dataunits::GridBox *apGridBox, int time_step);

                void
                BeforeShotStacking(dataunits::GridBox *apGridBox,
                                   dataunits::FrameBuffer<float> *shot_correlation);

                void
                AfterShotStacking(dataunits::GridBox *apGridBox,
                                  dataunits::FrameBuffer<float> *stacked_shot_correlation);

                void
                AfterMigration(dataunits::GridBox *apGridBox,
                               dataunits::FrameBuffer<float> *stacked_shot_correlation);

                std::vector<Callback *> &GetCallbacks();

            private:
                std::vector<Callback *> callbacks;
            };
        } //namespace callbacks
    } //namespace operations
} //namespace operations

#endif // OPERATIONS_LIB_CALLBACK_COLLECTION_HPP
