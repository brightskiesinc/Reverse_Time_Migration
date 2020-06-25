//
// Created by amrnasr on 13/11/2019.
//

#include "binary_trace_manager.h"
#include <cmath>
#include <skeleton/helpers/dout/dout.h>
#include <skeleton/helpers/memory_allocation/memory_allocator.h>

BinaryTraceManager::BinaryTraceManager() {
  this->absolute_shot_num = 0;
  traces = new Traces();
  traces->traces = nullptr;
  this->trace_file = nullptr;
}

BinaryTraceManager::~BinaryTraceManager() {
  if (trace_file != nullptr) {
    delete trace_file;
  }
  if (traces->traces != nullptr) {
    mem_free(traces->traces);
  }
  delete traces;
}

Point3D BinaryTraceManager::DeLocalizePoint(Point3D point, bool is_2D,
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

void BinaryTraceManager::ReadShot(vector<string> filenames, uint shot_number, string sort_key) {
  if (trace_file != nullptr) {
    delete trace_file;
    trace_file = nullptr;
  }
  if (traces->traces != nullptr) {
    mem_free(traces->traces);
    traces->traces = nullptr;
  }
  if (shot_number >= filenames.size()) {
      cout << "Invalid shot number given : " << shot_number << std::endl;
      exit(0);
  }
  trace_file =
      new ifstream(filenames[shot_number], ios::out | ios::binary);
  if (!trace_file->is_open()) {
      cout << "Couldn't open trace file '" << filenames[shot_number] << "'..." << std::endl;
      exit(0);
  }
  this->absolute_shot_num++;
  trace_file->read((char *)&this->source_point, sizeof(this->source_point));
  trace_file->read((char *)&this->r_start, sizeof(this->r_start));
  trace_file->read((char *)&this->r_inc, sizeof(this->r_inc));
  trace_file->read((char *)&this->r_end, sizeof(this->r_end));
  trace_file->read((char *)&total_time, sizeof(total_time));
  trace_file->read((char *)&traces->sample_dt, sizeof(traces->sample_dt));
  uint num_elements_per_time_step = 0;
  uint num_rec_x = 0;
  uint num_rec_y = 0;
  uint x_inc = r_inc.x == 0 ? 1 : r_inc.x;
  uint y_inc = r_inc.y == 0 ? 1 : r_inc.y;
  uint z_inc = r_inc.z == 0 ? 1 : r_inc.z;
  for (int iz = r_start.z; iz < r_end.z; iz += z_inc) {
    num_rec_y = 0;
    for (int iy = r_start.y; iy < r_end.y; iy += y_inc) {
      num_rec_y++;
      num_rec_x = 0;
      for (int ix = r_start.x; ix < r_end.x; ix += x_inc) {
        num_elements_per_time_step++;
        num_rec_x++;
      }
    }
  }
  traces->trace_size_per_timestep = num_elements_per_time_step;
  traces->num_receivers_in_x = num_rec_x;
  traces->num_receivers_in_y = num_rec_y;
  int sample_nt = int(total_time / traces->sample_dt) - 1;
  traces->sample_nt = sample_nt;
  traces->traces = (float *)mem_allocate(
      sizeof(float), sample_nt * num_elements_per_time_step, "traces");
  for (int t = 0; t < sample_nt; t++) {
    int index = 0;
    for (int iz = r_start.z; iz < r_end.z; iz += z_inc) {
      for (int iy = r_start.y; iy < r_end.y; iy += y_inc) {
        for (int ix = r_start.x; ix < r_end.x; ix += x_inc) {
          float value = 0;
          trace_file->read((char *)&value, sizeof(value));
          traces->traces[t * num_elements_per_time_step + index] = value;
          index++;
        }
      }
    }
  }
  grid->nt = int(total_time / grid->dt);
}

void BinaryTraceManager::PreprocessShot(uint cut_off_timestep) {
  bool is_2D = grid->grid_size.ny == 1;
  uint half_length = parameters->half_length;
  uint bound_length = parameters->boundary_length;
  this->source_point =
      DeLocalizePoint(this->source_point, is_2D, half_length, bound_length);
  this->r_start =
      DeLocalizePoint(this->r_start, is_2D, half_length, bound_length);
  this->r_end = DeLocalizePoint(this->r_end, is_2D, half_length, bound_length);

  int x_inc = r_inc.x == 0 ? 1 : r_inc.x;
  int y_inc = r_inc.y == 0 ? 1 : r_inc.y;
  int trace_size = traces->trace_size_per_timestep;
  int wnx = grid->window_size.window_nx;
  int wnz_wnx = grid->window_size.window_nz * wnx;
  /*
      // Muting for 2D, not implemented for 3D.
      if (is_2D) {
          float *travel_times = new float[wnx];
          float dt = grid->dt;
          float dx = grid->cell_dimensions.dx;
          for (int i = 0; i < wnx; i++) {
              // Recalculate velocity from the precomputed c2 * dt2
              float velocity = grid->velocity[i + (half_length + bound_length) *
     wnx
                                              +
     grid->window_size.window_start.x]; velocity = velocity / (dt * dt);
              velocity = sqrt(velocity);
              travel_times[i] = dx / velocity;
          }

          for (int i = this->source_point.x + 1; i < wnx - half_length; i++) {
              travel_times[i] = travel_times[i] + travel_times[i - 1];
          }

          for (int i = this->source_point.x - 1; i >= half_length; i--) {
              travel_times[i] = travel_times[i] + travel_times[i + 1];
          }
          int index = 0;
          int sample_nt = int(total_time / traces->sample_dt) - 1;
          for (int iy = r_start.y; iy < r_end.y; iy += y_inc) {
              for (int ix = r_start.x; ix < r_end.x; ix += x_inc) {
                  uint num_times = cut_off_timestep + uint(travel_times[ix] /
     traces->sample_dt); if (num_times > sample_nt) { num_times = sample_nt;
                  }
                  for (uint it = 0; it < num_times; it++) {
                      traces->traces[it * trace_size + index] = 0;
                  }
                  index++;
              }
          }
          delete[] travel_times;
      }
  */
}

void BinaryTraceManager::ApplyTraces(uint time_step) {
  int x_inc = r_inc.x == 0 ? 1 : r_inc.x;
  int y_inc = r_inc.y == 0 ? 1 : r_inc.y;
  int z_inc = r_inc.z == 0 ? 1 : r_inc.z;
  int trace_size = traces->trace_size_per_timestep;
  int wnx = grid->window_size.window_nx;
  int wnz_wnx = grid->window_size.window_nz * wnx;
  int index = 0;
  float current_time = (time_step - 1) * grid->dt;
  uint trace_step = uint(current_time / traces->sample_dt);
  trace_step = min(trace_step, traces->sample_nt - 1);
  for (int iz = r_start.z; iz < r_end.z; iz += z_inc) {
    for (int iy = r_start.y; iy < r_end.y; iy += y_inc) {
      for (int ix = r_start.x; ix < r_end.x; ix += x_inc) {
        int offset = iy * wnz_wnx + iz * wnx + ix;
        grid->pressure_current[offset] +=
            traces->traces[(trace_step)*trace_size + index];
        index++;
      }
    }
  }
}

Traces *BinaryTraceManager::GetTraces() { return traces; }

void BinaryTraceManager::SetComputationParameters(
    ComputationParameters *parameters) {
  this->parameters = (AcousticOmpComputationParameters *)(parameters);
  if (this->parameters == nullptr) {
    std::cout << "Not a compatible computation parameters : "
                 "expected AcousticOmpComputationParameters"
              << std::endl;
    exit(-1);
  }
}

void BinaryTraceManager::SetGridBox(GridBox *grid_box) {
  this->grid = grid_box;
}

Point3D *BinaryTraceManager::GetSourcePoint() { return &source_point; }

vector<uint> BinaryTraceManager::GetWorkingShots(vector<string> filenames, uint min_shot, uint max_shot, string type) {
    vector<uint> all_shots;
    uint end_val = max_shot < filenames.size() - 1 ? max_shot : filenames.size() - 1;;
    for (uint i = min_shot; i <= end_val; i++) {
        all_shots.push_back(i);
    }
    return all_shots;
}
