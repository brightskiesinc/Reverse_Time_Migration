//
// Created by amr on 11/11/2019.
//

#include "dout_callback.h"

void DoutCallback::BeforeInitialization(ComputationParameters *parameters) {
  dout << "Before init : Frequency is " << parameters->source_frequency << endl;
}

void DoutCallback::AfterInitialization(GridBox *box) {
  dout << "After init : nx " << box->grid_size.nx << endl;
  dout << "After init : ny " << box->grid_size.ny << endl;
  dout << "After init : nz " << box->grid_size.nz << endl;
}

void DoutCallback::BeforeShotPreprocessing(Traces *traces) {
  dout << "Before shot preprocessing" << endl;
}

void DoutCallback::AfterShotPreprocessing(Traces *traces) {
  dout << "After shot preprocessing" << endl;
}

void DoutCallback::BeforeForwardPropagation(GridBox *box) {
  dout << "Before forward propagation" << endl;
}

void DoutCallback::AfterForwardStep(GridBox *box, uint time_step) {
  dout << "Forward Step : " << time_step << endl;
}

void DoutCallback::BeforeBackwardPropagation(GridBox *box) {
  dout << "Before backward propagation" << endl;
}

void DoutCallback::AfterBackwardStep(GridBox *box, uint time_step) {
  dout << "Backward Step : " << time_step << endl;
}

void DoutCallback::AfterFetchStep(GridBox *forward_collector_box,
                                  uint time_step) {
  dout << "Reverse Step : " << time_step << endl;
}

void DoutCallback::BeforeShotStacking(float *shot_correlation,
                                      GridBox *meta_data) {
  dout << "Before shot stacking" << endl;
}

void DoutCallback::AfterShotStacking(float *stacked_shot_correlation,
                                     GridBox *meta_data) {
  dout << "After shot stacking" << endl;
}

void DoutCallback::AfterMigration(float *stacked_shot_correlation,
                                  GridBox *meta_data) {
  dout << "After Migration" << endl;
}
