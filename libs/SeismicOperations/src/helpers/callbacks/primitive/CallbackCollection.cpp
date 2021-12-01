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

#include <operations/helpers/callbacks/primitive/CallbackCollection.hpp>

using namespace std;
using namespace operations::helpers::callbacks;
using namespace operations::common;
using namespace operations::dataunits;


void CallbackCollection::RegisterCallback(Callback *apCallback) {
    this->callbacks.push_back(apCallback);
}

void CallbackCollection::BeforeInitialization(
        ComputationParameters *apParameters) {
    for (auto it : this->callbacks) {
        it->BeforeInitialization(apParameters);
    }
}

void CallbackCollection::AfterInitialization(GridBox *apGridBox) {
    for (auto it : this->callbacks) {
        it->AfterInitialization(apGridBox);
    }
}

void CallbackCollection::BeforeShotPreprocessing(TracesHolder *apTraces) {
    for (auto it : this->callbacks) {
        it->BeforeShotPreprocessing(apTraces);
    }
}

void CallbackCollection::AfterShotPreprocessing(TracesHolder *apTraces) {
    for (auto it : this->callbacks) {
        it->AfterShotPreprocessing(apTraces);
    }
}

void CallbackCollection::BeforeForwardPropagation(GridBox *apGridBox) {
    for (auto it : this->callbacks) {
        it->BeforeForwardPropagation(apGridBox);
    }
}

void CallbackCollection::AfterForwardStep(GridBox *apGridBox, int time_step) {
    for (auto it : this->callbacks) {
        it->AfterForwardStep(apGridBox, time_step);
    }
}

void CallbackCollection::BeforeBackwardPropagation(GridBox *apGridBox) {
    for (auto it : this->callbacks) {
        it->BeforeBackwardPropagation(apGridBox);
    }
}

void CallbackCollection::AfterBackwardStep(
        GridBox *apGridBox, int time_step) {
    for (auto it : this->callbacks) {
        it->AfterBackwardStep(apGridBox, time_step);
    }
}

void CallbackCollection::AfterFetchStep(
        GridBox *apGridBox, int time_step) {
    for (auto it : this->callbacks) {
        it->AfterFetchStep(apGridBox, time_step);
    }
}

void CallbackCollection::BeforeShotStacking(
        GridBox *apGridBox, FrameBuffer<float> *shot_correlation) {
    for (auto it : this->callbacks) {
        it->BeforeShotStacking(apGridBox, shot_correlation);
    }
}

void CallbackCollection::AfterShotStacking(
        GridBox *apGridBox, FrameBuffer<float> *stacked_shot_correlation) {
    for (auto it : this->callbacks) {
        it->AfterShotStacking(apGridBox, stacked_shot_correlation);
    }
}

void CallbackCollection::AfterMigration(
        GridBox *apGridBox, FrameBuffer<float> *stacked_shot_correlation) {
    for (auto it : this->callbacks) {
        it->AfterMigration(apGridBox, stacked_shot_correlation);
    }
}

vector<Callback *> &CallbackCollection::GetCallbacks() {
    return this->callbacks;
}
