//
// Created by amr on 11/11/2019.
//

#ifndef RTM_FRAMEWORK_DOUT_CALLBACK_H
#define RTM_FRAMEWORK_DOUT_CALLBACK_H

#include "../callback.h"
#include <skeleton/helpers/dout/dout.h>

class DoutCallback : public Callback {
public:
  void BeforeInitialization(ComputationParameters *parameters) override;

  void AfterInitialization(GridBox *box) override;

  void BeforeShotPreprocessing(Traces *traces) override;

  void AfterShotPreprocessing(Traces *traces) override;

  void BeforeForwardPropagation(GridBox *box) override;

  void AfterForwardStep(GridBox *box, uint time_step) override;

  void BeforeBackwardPropagation(GridBox *box) override;

  void AfterBackwardStep(GridBox *box, uint time_step) override;

  void AfterFetchStep(GridBox *forward_collector_box, uint time_step) override;

  void BeforeShotStacking(float *shot_correlation, GridBox *meta_data) override;

  void AfterShotStacking(float *stacked_shot_correlation,
                         GridBox *meta_data) override;

  void AfterMigration(float *stacked_shot_correlation,
                      GridBox *meta_data) override;
};
#endif // RTM_FRAMEWORK_DOUT_CALLBACK_H
