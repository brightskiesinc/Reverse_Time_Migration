//
// Created by ingy on 1/28/20.
//

#include "seismic_trace_manager.h"
#include <cmath>
#include <seismic-io-framework/datatypes.h>
#include <skeleton/helpers/dout/dout.h>
#include <skeleton/helpers/memory_allocation/memory_allocator.h>
#include <read_utils.h>
#include <utility>
#include <skeleton/helpers/timer/timer.hpp>


SeismicTraceManager::SeismicTraceManager() {
  traces = new Traces();
  traces->traces = nullptr;
  x_positions = nullptr;
  y_positions = nullptr;
  sio = nullptr;
  IO = new SEGYIOManager();
}

SeismicTraceManager::~SeismicTraceManager() {
  if (traces->traces != nullptr) {
    mem_free(traces->traces);
    mem_free(x_positions);
    mem_free(y_positions);
  }
  delete IO;
  delete traces;
}

Point3D SeismicTraceManager::SDeLocalizePoint(Point3D point, bool is_2D,
                                              uint half_length,
                                              uint bound_length) {
  Point3D copy;
  copy.x = point.x + half_length + bound_length;
  copy.z = point.z + half_length + bound_length;
  if (!is_2D) {
    copy.y = point.y + half_length + bound_length;
  } else {
    copy.y = point.y;
  }
  return copy;
}

IPoint3D SeismicTraceManager::DeLocalizePointS(IPoint3D point, bool is_2D,
                                               uint half_length,
                                               uint bound_length) {
  IPoint3D copy;
  copy.x = point.x + half_length + bound_length;
  copy.z = point.z + half_length + bound_length;
  if (!is_2D) {
    copy.y = point.y + half_length + bound_length;
  } else {
    copy.y = point.y;
  }
  return copy;
}

void SeismicTraceManager::ReadShot(vector<string> filenames, uint shot_number, string sort_key) {
  if (traces->traces != nullptr) {
    mem_free(traces->traces);
    mem_free(x_positions);
    mem_free(y_positions);
    traces->traces = nullptr;
    x_positions = nullptr;
    y_positions = nullptr;
  }

  bool found = false;
  string file_name;
  if (shot_to_file_mapping.find(shot_number) != shot_to_file_mapping.end()) {
      file_name = shot_to_file_mapping[shot_number];
      found = true;
  }

  if (!found) {
    std::cout << "Didn't find a suitable file to read shot ID " << shot_number
              << " from..." << std::endl;
    exit(0);
  } else {
    std::cout << "Reading trace: " << file_name << " for shot ID " << shot_number
              << std::endl;
  }
  sio = new SeIO();
  Timer *timer = Timer::getInstance();
  timer->start_timer("IO::ReadSelectedShotFromSegyFile");
  IO->ReadSelectiveDataFromFile(file_name, sort_key, sio, shot_number);
  timer->stop_timer("IO::ReadSelectedShotFromSegyFile");
  ParseSeioToTraces(sio, &this->source_point, this->traces, &x_positions, &y_positions,
                      this->grid, this->parameters, &this->total_time);
  delete sio;
}
void SeismicTraceManager::PreprocessShot(uint cut_off_timestep) {
  bool is_2D = grid->grid_size.ny == 1;
  uint half_length = parameters->half_length;
  uint bound_length = parameters->boundary_length;
  this->source_point =
      SDeLocalizePoint(this->source_point, is_2D, half_length, bound_length);
}

void SeismicTraceManager::ApplyTraces(uint time_step) {
  int trace_size = traces->trace_size_per_timestep;
  int wnx = grid->window_size.window_nx;
  int wnz_wnx = grid->window_size.window_nz * wnx;
  int std_offset = (parameters->boundary_length + parameters->half_length) * wnx;
  float current_time = (time_step - 1) * grid->dt;
  float dt = grid->dt;

  uint trace_step = uint(current_time / traces->sample_dt);
  trace_step = min(trace_step, traces->sample_nt - 1);

  for (int i = 0; i < trace_size; i++) {
      int offset = y_positions[i] * wnz_wnx + std_offset + x_positions[i];
      grid->pressure_current[offset] +=
              traces->traces[(trace_step)*trace_size + i] * grid->window_velocity[offset];

  }
}

Traces *SeismicTraceManager::GetTraces() { return traces; }

void SeismicTraceManager::SetComputationParameters(
    ComputationParameters *parameters) {
  this->parameters = (AcousticOmpComputationParameters *)(parameters);
  if (this->parameters == nullptr) {
    std::cout << "Not a compatible computation parameters : "
                 "expected AcousticOmpComputationParameters"
              << std::endl;
    exit(-1);
  }
}

void SeismicTraceManager::SetGridBox(GridBox *grid_box) {
  this->grid = grid_box;
}

Point3D *SeismicTraceManager::GetSourcePoint() { return &source_point; }

vector<uint> SeismicTraceManager::GetWorkingShots(vector<string> filenames, uint min_shot, uint max_shot, string type) {
    vector<uint> all_shots;
    for (string filename : filenames) {
        vector<uint> file_ids = IO->GetUniqueOccurences(filename, type, min_shot, max_shot);
        for (uint shot_id : file_ids) {
            all_shots.push_back(shot_id);
            shot_to_file_mapping[shot_id] = filename;
        }
    }
    return all_shots;
}
