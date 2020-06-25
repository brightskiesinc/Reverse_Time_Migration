//
// Created by amr on 05/11/19.
//

#ifndef RTM_FRAMEWORK_CALLBACK_COLLECTION_H
#define RTM_FRAMEWORK_CALLBACK_COLLECTION_H

#include "callback.h"
#include <vector>

using namespace std;

class CallbackCollection {
private:
  vector<Callback *> callbacks;

public:
  void RegisterCallback(Callback *callback);
  void BeforeInitialization(ComputationParameters *parameters);
  void AfterInitialization(GridBox *box);
  void BeforeShotPreprocessing(Traces *traces);
  void AfterShotPreprocessing(Traces *traces);
  void BeforeForwardPropagation(GridBox *box);
  void AfterForwardStep(GridBox *box, uint time_step);
  void BeforeBackwardPropagation(GridBox *box);
  void AfterBackwardStep(GridBox *box, uint time_step);
  void AfterFetchStep(GridBox *forward_collector_box, uint time_step);
  void BeforeShotStacking(float *shot_correlation, GridBox *meta_data);
  void AfterShotStacking(float *stacked_shot_correlation, GridBox *meta_data);
  void AfterMigration(float *stacked_shot_correlation, GridBox *meta_data);
};

#endif // RTM_FRAMEWORK_CALLBACK_COLLECTION_H
