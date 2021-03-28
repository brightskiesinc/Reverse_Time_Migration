//
// Created by amr-nasr on 05/11/19.
//

#include "operations/helpers/callbacks/primitive/CallbackCollection.hpp"

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

void CallbackCollection::AfterForwardStep(GridBox *apGridBox, uint time_step) {
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
        GridBox *apGridBox, uint time_step) {
    for (auto it : this->callbacks) {
        it->AfterBackwardStep(apGridBox, time_step);
    }
}

void CallbackCollection::AfterFetchStep(
        GridBox *apGridBox, uint time_step) {
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
