//
// Created by amrnasr on 13/11/2019.
//

#ifndef ACOUSTIC2ND_RTM_BINARY_TRACE_MANAGER_ONEAPI_H
#define ACOUSTIC2ND_RTM_BINARY_TRACE_MANAGER_ONEAPI_H

#include <CL/sycl.hpp>
#include <concrete-components/data_units/acoustic_dpc_computation_parameters.h>
#include <concrete-components/data_units/acoustic_second_grid.h>
#include <skeleton/components/trace_manager.h>

#include <fstream>

using namespace std;
using namespace cl::sycl;

class BinaryTraceManager : public TraceManager {
  ifstream *trace_file;
  Traces traces;
  Point3D r_start;
  Point3D r_end;
  Point3D r_inc;
  uint absolute_shot_num;
  float total_time;
  AcousticSecondGrid *grid;
  AcousticDpcComputationParameters *parameters;
  Point3D source_point;
  float *ptr_traces;

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

#endif // ACOUSTIC2ND_RTM_BINARY_TRACE_MANAGER_ONEAPI_H
