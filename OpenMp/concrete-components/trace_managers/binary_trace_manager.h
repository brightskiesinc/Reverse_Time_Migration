//
// Created by amrnasr on 13/11/2019.
//

#ifndef ACOUSTIC2ND_RTM_BINARY_TRACE_MANAGER_H
#define ACOUSTIC2ND_RTM_BINARY_TRACE_MANAGER_H

#include <concrete-components/data_units/acoustic_openmp_computation_parameters.h>
#include <concrete-components/data_units/acoustic_second_grid.h>
#include <skeleton/components/trace_manager.h>

#include <fstream>

using namespace std;

class BinaryTraceManager : public TraceManager {
  ifstream *trace_file;
  Traces *traces;
  Point3D r_start;
  Point3D r_end;
  Point3D r_inc;
  uint absolute_shot_num;
  float total_time;
  GridBox *grid;
  AcousticOmpComputationParameters *parameters;
  Point3D source_point;
  Point3D DeLocalizePoint(Point3D point, bool is_2D, uint half_length,
                          uint bound_length);

public:
  BinaryTraceManager();
  ~BinaryTraceManager() override;
  void ReadShot(vector<string> filenames, uint shot_number, string sort_key) override;
  void PreprocessShot(uint cut_off_timestep) override;
  void ApplyTraces(uint time_step) override;
  Traces *GetTraces() override;
  void SetComputationParameters(ComputationParameters *parameters) override;
  void SetGridBox(GridBox *grid_box) override;
  Point3D *GetSourcePoint() override;

  vector<uint> GetWorkingShots(vector<string> filenames, uint min_shot, uint max_shot, string type) override;
};

#endif // ACOUSTIC2ND_RTM_BINARY_TRACE_MANAGER_H
