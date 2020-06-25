#ifndef ACOUSTIC2ND_RTM_WRITER_CALLBACK_H
#define ACOUSTIC2ND_RTM_WRITER_CALLBACK_H

#include <skeleton/helpers/callbacks/callback.h>
#include <string>
#include <write_utils.h>

using namespace std;

class WriterCallback : public Callback {
private:
  uint show_each;
  uint shot_num;
  bool write_velocity;
  bool write_forward;
  bool write_backward;
  bool write_reverse;
  bool write_migration;
  bool write_re_extended_velocity;
  bool write_single_shot_correlation;
  bool write_each_stacked_shot;
  bool write_traces_raw;
  bool write_traces_preprocessed;
  string write_path;

public:
  WriterCallback(uint show_each, bool write_velocity, bool write_forward,
             bool write_backward, bool write_reverse, bool write_migration,
             bool write_re_extended_velocity,
             bool write_single_shot_correlation, bool write_each_stacked_shot,
             bool write_traces_raw, bool writer_traces_preprocessed,
             string write_path, string folder_name);
  virtual string GetExtension() = 0;
  virtual void WriteResult(uint nx, uint nz, uint nt, uint ny, float dx, float dz, float dt,
                         float dy, float *data, std::string filename, bool is_traces) = 0;
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

#endif // ACOUSTIC2ND_RTM_WRITER_CALLBACK_H
