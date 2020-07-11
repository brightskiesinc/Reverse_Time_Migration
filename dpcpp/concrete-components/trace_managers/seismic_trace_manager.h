//
// Created by ingy on 1/28/20.
//

#ifndef ACOUSTIC2ND_RTM_SEISMIC_TRACE_MANAGER_H
#define ACOUSTIC2ND_RTM_SEISMIC_TRACE_MANAGER_H
#include <IO/io_manager.h>
#include <Segy/segy_io_manager.h>
#include <concrete-components/data_units/acoustic_dpc_computation_parameters.h>
#include <concrete-components/data_units/acoustic_second_grid.h>
#include <datatypes.h>
#include <fstream>
#include <skeleton/components/trace_manager.h>
#include <unordered_map>

using namespace std;

class SeismicTraceManager : public TraceManager {
private:
  IOManager *IO;
  SeIO *sio;
  unordered_map<uint, string> shot_to_file_mapping;
  Traces *traces;

  float total_time;
  AcousticSecondGrid *grid;
  AcousticDpcComputationParameters *parameters;
  Point3D source_point;

  float *ptr_traces;
  uint *x_positions;
  uint *y_positions;

  Point3D SDeLocalizePoint(Point3D point, bool is_2D, uint half_length,
                           uint bound_length);
  IPoint3D DeLocalizePointS(IPoint3D point, bool is_2D, uint half_length,
                            uint bound_length);

public:

  SeismicTraceManager();
  ~SeismicTraceManager() override;
  void ReadShot(vector<string> filenames, uint shot_number, string sort_key) override;
  void PreprocessShot(uint cut_off_timestep) override;
  void ApplyTraces(uint time_step) override;
  Traces *GetTraces() override;
  void SetComputationParameters(ComputationParameters *parameters) override;
  void SetGridBox(GridBox *grid_box) override;
  Point3D *GetSourcePoint() override;
  vector<uint> GetWorkingShots(vector<string> filenames, uint min_shot, uint max_shot, string type) override;

};

#endif // ACOUSTIC2ND_RTM_SEISMIC_TRACE_MANAGER_H
