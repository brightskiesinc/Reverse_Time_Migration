//
// Created by amr on 18/01/2020.
//

#ifndef ACOUSTIC2ND_RTM_NORM_WRITER_H
#define ACOUSTIC2ND_RTM_NORM_WRITER_H

#include <fstream>
#include <skeleton/helpers/callbacks/callback.h>
#include <string>

using namespace std;

class NormWriter : public Callback {
private:
  uint show_each;
  bool write_forward;
  bool write_backward;
  bool write_reverse;
  string write_path;
  static float GetNorm(float *mat, uint nx, uint nz, uint ny);
  ofstream *forward_norm_stream;
  ofstream *reverse_norm_stream;
  ofstream *backward_norm_stream;

public:
  NormWriter(uint show_each, bool write_forward, bool write_backward,
             bool write_reverse, string write_path);

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

  ~NormWriter() {
    if (this->write_forward) {
      delete forward_norm_stream;
    }
    if (this->write_reverse) {
      delete reverse_norm_stream;
    }
    if (this->write_backward) {
      delete backward_norm_stream;
    }
  }
};

#endif // ACOUSTIC2ND_RTM_NORM_WRITER_H
