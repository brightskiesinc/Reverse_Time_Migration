//
// Created by amrnasr on 02/11/2019.
//

#ifndef RTM_FRAMEWORK_CALLBACK_H
#define RTM_FRAMEWORK_CALLBACK_H

#include <skeleton/base/datatypes.h>
#include <skeleton/components/trace_manager.h>

class Callback {
public:
  virtual void BeforeInitialization(ComputationParameters *parameters) = 0;
  virtual void AfterInitialization(GridBox *box) = 0;
  virtual void BeforeShotPreprocessing(Traces *traces) = 0;
  virtual void AfterShotPreprocessing(Traces *traces) = 0;
  virtual void BeforeForwardPropagation(GridBox *box) = 0;
  virtual void AfterForwardStep(GridBox *box, uint time_step) = 0;
  virtual void BeforeBackwardPropagation(GridBox *box) = 0;
  virtual void AfterBackwardStep(GridBox *box, uint time_step) = 0;
  virtual void AfterFetchStep(GridBox *forward_collector_box,
                              uint time_step) = 0;
  virtual void BeforeShotStacking(float *shot_correlation,
                                  GridBox *meta_data) = 0;
  virtual void AfterShotStacking(float *stacked_shot_correlation,
                                 GridBox *meta_data) = 0;
  virtual void AfterMigration(float *stacked_shot_correlation,
                              GridBox *meta_data) = 0;
};

#endif // RTM_FRAMEWORK_CALLBACK_H
