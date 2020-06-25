//
// Created by amr on 05/11/19.
//
#include "callback_collection.h"

void CallbackCollection::RegisterCallback(Callback *callback) {
  this->callbacks.push_back(callback);
}

void CallbackCollection::BeforeInitialization(
    ComputationParameters *parameters) {
  for (auto it : this->callbacks) {
    it->BeforeInitialization(parameters);
  }
}

void CallbackCollection::AfterInitialization(GridBox *box) {
  for (auto it : this->callbacks) {
    it->AfterInitialization(box);
  }
}

void CallbackCollection::BeforeShotPreprocessing(Traces *traces) {
  for (auto it : this->callbacks) {
    it->BeforeShotPreprocessing(traces);
  }
}

void CallbackCollection::AfterShotPreprocessing(Traces *traces) {
  for (auto it : this->callbacks) {
    it->AfterShotPreprocessing(traces);
  }
}

void CallbackCollection::BeforeForwardPropagation(GridBox *box) {
  for (auto it : this->callbacks) {
    it->BeforeForwardPropagation(box);
  }
}

void CallbackCollection::AfterForwardStep(GridBox *box, uint time_step) {
  for (auto it : this->callbacks) {
    it->AfterForwardStep(box, time_step);
  }
}

void CallbackCollection::BeforeBackwardPropagation(GridBox *box) {
  for (auto it : this->callbacks) {
    it->BeforeBackwardPropagation(box);
  }
}

void CallbackCollection::AfterBackwardStep(GridBox *box, uint time_step) {
  for (auto it : this->callbacks) {
    it->AfterBackwardStep(box, time_step);
  }
}

void CallbackCollection::AfterFetchStep(GridBox *forward_collector_box,
                                        uint time_step) {
  for (auto it : this->callbacks) {
    it->AfterFetchStep(forward_collector_box, time_step);
  }
}

void CallbackCollection::BeforeShotStacking(float *shot_correlation,
                                            GridBox *meta_data) {
  for (auto it : this->callbacks) {
    it->BeforeShotStacking(shot_correlation, meta_data);
  }
}

void CallbackCollection::AfterShotStacking(float *stacked_shot_correlation,
                                           GridBox *meta_data) {
  for (auto it : this->callbacks) {
    it->AfterShotStacking(stacked_shot_correlation, meta_data);
  }
}

void CallbackCollection::AfterMigration(float *stacked_shot_correlation,
                                        GridBox *meta_data) {
  for (auto it : this->callbacks) {
    it->AfterMigration(stacked_shot_correlation, meta_data);
  }
}
