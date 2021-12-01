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

#ifndef OPERATIONS_LIB_CALLBACK_HPP
#define OPERATIONS_LIB_CALLBACK_HPP

#include <operations/common/DataTypes.h>
#include <operations/common/ComputationParameters.hpp>
#include <operations/data-units/concrete/holders/GridBox.hpp>
#include <operations/data-units/concrete/holders/TracesHolder.hpp>

namespace operations {
    namespace helpers {
        namespace callbacks {

            class Callback {
            public:
                virtual void
                BeforeInitialization(common::ComputationParameters *apParameters) = 0;

                virtual void
                AfterInitialization(dataunits::GridBox *apGridBox) = 0;

                virtual void
                BeforeShotPreprocessing(dataunits::TracesHolder *apTraces) = 0;

                virtual void
                AfterShotPreprocessing(dataunits::TracesHolder *apTraces) = 0;

                virtual void
                BeforeForwardPropagation(dataunits::GridBox *apGridBox) = 0;

                virtual void
                AfterForwardStep(dataunits::GridBox *apGridBox, int time_step) = 0;

                virtual void
                BeforeBackwardPropagation(dataunits::GridBox *apGridBox) = 0;

                virtual void
                AfterBackwardStep(dataunits::GridBox *apGridBox, int time_step) = 0;

                virtual void
                AfterFetchStep(dataunits::GridBox *apGridBox, int time_step) = 0;

                virtual void
                BeforeShotStacking(dataunits::GridBox *apGridBox, dataunits::FrameBuffer<float> *shot_correlation) = 0;

                virtual void
                AfterShotStacking(dataunits::GridBox *apGridBox,
                                  dataunits::FrameBuffer<float> *stacked_shot_correlation) = 0;

                virtual void
                AfterMigration(dataunits::GridBox *apGridBox,
                               dataunits::FrameBuffer<float> *stacked_shot_correlation) = 0;
            };
        } //namespace callbacks
    } //namespace operations
} //namespace operations

#endif // OPERATIONS_LIB_CALLBACK_HPP
