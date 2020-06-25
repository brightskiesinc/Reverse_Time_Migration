//
// Created by amrnasr on 13/11/2019.
//

#include "binary_trace_manager_oneApi.h"
#include <cmath>
#include <skeleton/helpers/dout/dout.h>
#include <skeleton/helpers/memory_allocation/memory_allocator.h>

BinaryTraceManager::BinaryTraceManager() {
  this->absolute_shot_num = 0;
  traces.traces = nullptr;
  this->trace_file = nullptr;
  ptr_traces = nullptr;
  parameters = nullptr;
  grid = nullptr;
  total_time = 0;
}

BinaryTraceManager::~BinaryTraceManager() {
  if (trace_file != nullptr) {
    delete trace_file;
  }
  if (traces.traces != nullptr) {
    mem_free(traces.traces);
  }
}

Point3D DeLocalizePoint(Point3D point, bool is_2D, uint half_length,
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
  }
  if (traces.traces != nullptr) {
    mem_free(traces.traces);
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
  trace_file->read((char *)&traces.sample_dt, sizeof(traces.sample_dt));
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
  traces.trace_size_per_timestep = num_elements_per_time_step;
  traces.num_receivers_in_x = num_rec_x;
  traces.num_receivers_in_y = num_rec_y;
  int sample_nt = int(total_time / traces.sample_dt) - 1;
  traces.sample_nt = sample_nt;
  traces.traces = (float *)mem_allocate(
      sizeof(float), sample_nt * num_elements_per_time_step, "traces");
  for (int t = 0; t < sample_nt; t++) {
    int index = 0;
    for (int iz = r_start.z; iz < r_end.z; iz += z_inc) {
      for (int iy = r_start.y; iy < r_end.y; iy += y_inc) {
        for (int ix = r_start.x; ix < r_end.x; ix += x_inc) {
          float value = 0;
          trace_file->read((char *)&value, sizeof(value));
          traces.traces[t * num_elements_per_time_step + index] = value;
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
  int trace_size = traces.trace_size_per_timestep;
  int wnx = grid->window_size.window_nx;
  int wnz_wnx = grid->window_size.window_nz * wnx;
  // Muting for 2D, not implemented for 3D.
  //    if (is_2D) {
  //        float *travel_times = new float[wnx];
  //        float dt = grid->dt;
  //        float dx = grid->cell_dimensions.dx;
  //        for (int i = 0; i < wnx; i++) {
  //            // Recalculate velocity from the precomputed c2 * dt2
  //            float velocity = grid->velocity[i + (half_length + bound_length)
  //            * wnx
  //                                            +
  //                                            grid->window_size.window_start.x];
  //            velocity = velocity / (dt * dt);
  //            velocity = sqrt(velocity);
  //            travel_times[i] = dx / velocity;
  //        }
  //
  //        for (int i = this->source_point.x + 1; i < wnx - half_length; i++) {
  //            travel_times[i] = travel_times[i] + travel_times[i - 1];
  //        }
  //
  //        for (int i = this->source_point.x - 1; i >= half_length; i--) {
  //            travel_times[i] = travel_times[i] + travel_times[i + 1];
  //        }
  //        int index = 0;
  //        for (int iy = r_start.y; iy < r_end.y; iy += y_inc) {
  //            for (int ix = r_start.x; ix < r_end.x; ix += x_inc) {
  //                uint num_times = cut_off_timestep + uint(travel_times[ix] /
  //                traces.sample_dt); for (uint it = 0; it < num_times; it++) {
  //                    traces.traces[it * trace_size + index] = 0;
  //                }
  //                index++;
  //            }
  //        }
  //        delete[] travel_times;
  //    }
  unsigned long sizeTotal = traces.trace_size_per_timestep * traces.sample_nt;
  ptr_traces = (float *)cl::sycl::malloc_device(
      sizeof(float) * sizeTotal,
      AcousticDpcComputationParameters::device_queue->get_device(),
      AcousticDpcComputationParameters::device_queue->get_context());
  AcousticDpcComputationParameters::device_queue->submit([&](handler &cgh) {
    cgh.memcpy(ptr_traces, traces.traces, sizeof(float) * sizeTotal);
  });
  AcousticDpcComputationParameters::device_queue->wait();
}

void BinaryTraceManager::ApplyTraces(uint time_step) {
  int x_inc = r_inc.x == 0 ? 1 : r_inc.x;
  int y_inc = r_inc.y == 0 ? 1 : r_inc.y;
  int z_inc = r_inc.z == 0 ? 1 : r_inc.z;
  int trace_size = traces.trace_size_per_timestep;
  int wnx = grid->window_size.window_nx;
  int wnz_wnx = grid->window_size.window_nz * wnx;
  float current_time = (time_step - 1) * grid->dt;
  uint trace_step = uint(current_time / traces.sample_dt);
  if (trace_step > traces.sample_nt - 1) {
    trace_step = traces.sample_nt - 1;
  }
  int r_start_y = r_start.y, r_start_x = r_start.x, r_start_z = r_start.z;
  int trace_nx = traces.num_receivers_in_x;

  AcousticDpcComputationParameters::device_queue->submit([&](handler &cgh) {
    auto global_range =
        range<3>(1, traces.num_receivers_in_y, traces.num_receivers_in_x);
    auto local_range = range<3>(1, 1, 1);
    auto global_nd_range = nd_range<3>(global_range, local_range);

    float *current = grid->pressure_current;
    float *d_traces = ptr_traces;
    cgh.parallel_for<class trace_manager>(global_nd_range, [=](nd_item<3> it) {
      int iz = it.get_global_id(0);
      int iy = it.get_global_id(1);
      int ix = it.get_global_id(2);
      int offset = ((iy * y_inc) + r_start_y) * wnz_wnx +
                   ((iz * z_inc) + r_start_z) * wnx +
                   ((ix * x_inc) + r_start_x);
      current[offset] += d_traces[(trace_step)*trace_size + iy * trace_nx + ix];
    });
  });

  AcousticDpcComputationParameters::device_queue->wait();
}

Traces *BinaryTraceManager::GetTraces() { return &traces; }

void BinaryTraceManager::SetComputationParameters(
    ComputationParameters *parameters) {
  this->parameters = (AcousticDpcComputationParameters *)(parameters);
  if (this->parameters == nullptr) {
    std::cout << "Not a compatible computation parameters : "
                 "expected AcousticDpcComputationParameters"
              << std::endl;
    exit(-1);
  }
}

void BinaryTraceManager::SetGridBox(GridBox *grid_box) {
  this->grid = (AcousticSecondGrid *)(grid_box);
  if (this->grid == nullptr) {
    std::cout << "Not a compatible gridbox : "
                 "expected AcousticSecondGrid"
              << std::endl;
    exit(-1);
  }
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