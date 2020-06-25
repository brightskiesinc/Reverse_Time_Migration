//
// Created by ingy on 1/28/20.
//

#include "seismic_trace_manager.h"
#include <cmath>
#include <seismic-io-framework/datatypes.h>
#include <skeleton/helpers/dout/dout.h>
#include <skeleton/helpers/memory_allocation/memory_allocator.h>
#include <utility>

SeismicTraceManager::SeismicTraceManager() {
  this->absolute_shot_num = 0;
  traces = new Traces();
  traces->traces = nullptr;
  this->trace_file = nullptr;
  sio = nullptr;
  IO = new SEGYIOManager();
}

SeismicTraceManager::~SeismicTraceManager() {
  if (trace_file != nullptr) {
    delete trace_file;
  }
  if (traces->traces != nullptr) {
    mem_free(traces->traces);
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
  if (trace_file != nullptr) {
    delete trace_file;
    trace_file = nullptr;
  }
  if (traces->traces != nullptr) {
    mem_free(traces->traces);
    traces->traces = nullptr;
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
  IO->ReadSelectiveDataFromFile(file_name, sort_key, sio, shot_number);

  float scale = abs(sio->Atraces.at(0).TraceMetaData.scalar) * 1.0;

  // cout << "you are ere " << endl;
  this->source_point.x = (sio->Atraces.at(0).TraceMetaData.source_location_x) /
                         (grid->cell_dimensions.dx * scale);
  this->source_point.z = (sio->Atraces.at(0).TraceMetaData.source_location_z) /
                         (grid->cell_dimensions.dz * scale);
  this->source_point.y = (sio->Atraces.at(0).TraceMetaData.source_location_y) /
                         (grid->cell_dimensions.dy * scale);

  traces->sample_dt = sio->DM.dt;

  this->absolute_shot_num++;

  int sample_nt = sio->DM.nt;
  int total_rec_num = sio->Atraces.size();

  //cout << "sample nt " << sample_nt << endl;

  bool descent_x = false;
  bool descent_y = false;
  bool descent_z = false;

  IPoint3D first;
  IPoint3D last;
  IPoint3D second;

  first.x = (sio->Atraces.at(0).TraceMetaData.receiver_location_x -
             (int)grid->reference_point.x) /
            (grid->cell_dimensions.dx * scale);
  first.y = (sio->Atraces.at(0).TraceMetaData.receiver_location_y -
             (int)grid->reference_point.y) /
            (grid->cell_dimensions.dy * scale);
  first.z = (sio->Atraces.at(0).TraceMetaData.receiver_location_z -
             (int)grid->reference_point.z) /
            (grid->cell_dimensions.dz * scale);

  second.x = (sio->Atraces.at(1).TraceMetaData.receiver_location_x -
              (int)grid->reference_point.x) /
             (grid->cell_dimensions.dx * scale);
  second.y = (sio->Atraces.at(1).TraceMetaData.receiver_location_y -
              (int)grid->reference_point.y) /
             (grid->cell_dimensions.dy * scale);
  second.z = (sio->Atraces.at(1).TraceMetaData.receiver_location_z -
              (int)grid->reference_point.z) /
             (grid->cell_dimensions.dz * scale);

  last.x =
      (sio->Atraces.at(total_rec_num - 1).TraceMetaData.receiver_location_x -
       (int)grid->reference_point.x) /
      (grid->cell_dimensions.dx * scale);
  last.y =
      (sio->Atraces.at(total_rec_num - 1).TraceMetaData.receiver_location_y -
       (int)grid->reference_point.y) /
      (grid->cell_dimensions.dy * scale);
  last.z =
      (sio->Atraces.at(total_rec_num - 1).TraceMetaData.receiver_location_z -
       (int)grid->reference_point.z) /
      (grid->cell_dimensions.dz * scale);

  // setting the start and the end of the x locations of the receivers

  if (first.x < last.x) {

    this->r_start.x = first.x;
    this->r_end.x = last.x;

  } else {
    this->r_start.x = last.x;
    this->r_end.x = first.x;
    descent_x = true;
  }

  // setting the start and the end of the z locations of the receivers
  if (first.z < last.z) {

    this->r_start.z = first.z;
    this->r_end.z = last.z;
  } else {
    this->r_start.z = last.z;
    this->r_end.z = first.z;
    descent_z = true;
  }

  // setting the start and the end of the y locations of the receivers
  if (first.y < last.y) {

    this->r_start.y = first.y;
    this->r_end.y = last.y;
  } else {
    this->r_start.y = last.y;
    this->r_end.y = first.y;
    descent_y = true;
  }

  r_end.x = r_end.x <= 0 ? 1 : r_end.x;
  r_end.y = r_end.y <= 0 ? 1 : r_end.y;
  r_end.z = r_end.z <= 0 ? 1 : r_end.z;

  r_inc.x = abs(first.x - second.x);
  r_inc.y = abs(first.y - second.y);
  r_inc.z = abs(first.z - second.z);

  // cout << "rinx" << r_inc.y << endl;

  uint num_elements_per_time_step = 0;
  uint num_rec_x = 0;
  uint num_rec_y = 0;
  r_inc.x = r_inc.x == 0 ? 1 : r_inc.x;
  r_inc.y = r_inc.y == 0 ? 1 : r_inc.y;
  r_inc.z = r_inc.z == 0 ? 1 : r_inc.z;

  int iz_start = max(0, r_start.z);
  int ix_start = max(0, r_start.x);
  int iy_start = max(0, r_start.y);

  int iz_end = min(r_end.z, (int)grid->grid_size.nz);
  int ix_end = min(r_end.x, (int)grid->grid_size.nx);
  int iy_end = min(r_end.y, (int)grid->grid_size.ny);

  // cout << "startssss " <<  r_end.x << " "  << r_end.y << " " << r_end.z <<
  // endl;

  for (int iz = iz_start; iz < iz_end; iz += r_inc.z) {
    num_rec_y = 0;
    for (int iy = iy_start; iy < iy_end; iy += r_inc.y) {
      num_rec_y++;
      num_rec_x = 0;
      for (int ix = ix_start; ix < ix_end; ix += r_inc.x) {
        num_elements_per_time_step++;
        num_rec_x++;
      }
    }
  }

  // num_elements_per_time_step = (num_rec_x * r_inc.x ) + (num_rec_y *
  // r_inc.y);

  traces->trace_size_per_timestep = num_elements_per_time_step;
  traces->num_receivers_in_x = num_rec_x;
  traces->num_receivers_in_y = num_rec_y;

  traces->sample_nt = sample_nt;

  //     cout << "num steps " << num_elements_per_time_step << endl;

  traces->traces = (float *)mem_allocate(
      sizeof(float), sample_nt * num_elements_per_time_step, "traces");

  int rec_start_x;
  int rec_start_z;
  int rec_start_y;

  int rec_end_x;
  int rec_end_z;
  int rec_end_y;

  int diff_x = ix_start - r_start.x;
  int diff_y = iy_start - r_start.y;
  int diff_z = iz_start - r_start.z;

  if (!descent_x) {
    rec_start_x = diff_x / r_inc.x;
    rec_end_x = (ix_end - r_start.x) / r_inc.x;

  } else {

    rec_start_x = ix_start / r_inc.x;
    rec_end_x = (r_end.x - diff_x) / r_inc.x;
  }

  if (!descent_y) {
    rec_start_y = diff_y / r_inc.y;
    rec_end_y = (iy_end - r_start.y) / r_inc.y;
  } else {
    rec_start_y = iy_start / r_inc.y;
    rec_end_y = (r_end.y - diff_y) / r_inc.y;
  }

  if (!descent_z) {
    rec_start_z = diff_z / r_inc.z;
    rec_end_z = (iz_end - r_start.z) / r_inc.z;
  } else {
    rec_start_z = iz_start / r_inc.z;
    rec_end_z = (r_end.z - diff_z) / r_inc.z;
  }

  int total_rec_x = (last.x - first.x);

  // cout << "rec start " <<  rec_start_x << " rec end" << rec_end_x<< endl;

  int ind;

  for (int t = 0; t < sample_nt; t++) {
    int index = 0;
    for (int iz = rec_start_z; iz < rec_end_z; iz++) {
      for (int iy = rec_start_y; iy < rec_end_y; iy++) {
        for (int ix = rec_start_x; ix < rec_end_x; ix++) {
          float value = 0;

          index++;

          value = sio->Atraces.at(iy * num_rec_x + ix).TraceData[t];

          traces->traces[t * num_elements_per_time_step + index] = value;
        }
      }
    }
  }

  grid->nt =
      int(sample_nt * traces->sample_dt /
          grid->dt); // we dont have total time , but we have the nt from the
                     // segy file , so we can modify the nt accourding to the
                     // ratio between the recorded dt and the suitable dt

  total_time = sample_nt * traces->sample_dt;

  r_start.x = (sio->Atraces.at(rec_start_y * num_rec_x + rec_start_x)
                   .TraceMetaData.receiver_location_x -
               (int)grid->reference_point.x) /
              (grid->cell_dimensions.dx * scale);
  r_start.y = (sio->Atraces.at(rec_start_y * num_rec_x + rec_start_x)
                   .TraceMetaData.receiver_location_y -
               (int)grid->reference_point.y) /
              (grid->cell_dimensions.dy * scale);
  r_start.z = rec_start_z;

  r_end.x = (sio->Atraces.at((rec_end_y - 1) * num_rec_x + rec_end_x - 1)
                 .TraceMetaData.receiver_location_x -
             (int)grid->reference_point.x) /
            (grid->cell_dimensions.dx * scale);
  r_end.y = (sio->Atraces.at((rec_end_y - 1) * num_rec_x + rec_end_x - 1)
                 .TraceMetaData.receiver_location_y -
             (int)grid->reference_point.y) /
            (grid->cell_dimensions.dy * scale);
  r_end.z = rec_end_z;

  if (r_end.y == 0) {
    r_end.y = 1;
  }
  delete sio;
}
void SeismicTraceManager::PreprocessShot(uint cut_off_timestep) {
  bool is_2D = grid->grid_size.ny == 1;
  uint half_length = parameters->half_length;
  uint bound_length = parameters->boundary_length;
  this->source_point =
      SDeLocalizePoint(this->source_point, is_2D, half_length, bound_length);
  this->r_start =
      DeLocalizePointS(this->r_start, is_2D, half_length, bound_length);
  this->r_end = DeLocalizePointS(this->r_end, is_2D, half_length, bound_length);

  int x_inc = r_inc.x == 0 ? 1 : r_inc.x;
  int y_inc = r_inc.y == 0 ? 1 : r_inc.y;
  int trace_size = traces->trace_size_per_timestep;
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
  //
  //        for (int i = this->source_point.x + 1; i < wnx - half_length; i++) {
  //            travel_times[i] = travel_times[i] + travel_times[i - 1];
  //        }
  //
  //        for (int i = this->source_point.x - 1; i >= half_length; i--) {
  //            travel_times[i] = travel_times[i] + travel_times[i + 1];
  //        }
  //
  //        cout << "here in the preprocessing " << endl;
  //        int index = 0;
  //        int sample_nt = int(total_time / traces->sample_dt) - 1;
  //
  //        cout << "sample nt " << sample_nt << endl;
  //
  //        cout << "starts " << r_start.y << " " <<  r_start.x << endl;
  //
  //
  //
  //        for (int iy = r_start.y; iy < r_end.y; iy += y_inc) {
  //            for (int ix = r_start.x; ix < r_end.x; ix += x_inc) {
  //                uint num_times = cut_off_timestep + uint(travel_times[ix] /
  //                traces->sample_dt); if (num_times > sample_nt) {
  //                    num_times = sample_nt;
  //                }
  //                for (uint iAcousticSecondGridt = 0; it < num_times; it++) {
  //                    traces->traces[it * trace_size + index] = 0;
  //                }
  //                index++;
  //            }
  //        }
  //
  //        delete[] travel_times;
  //    }
}

void SeismicTraceManager::ApplyTraces(uint time_step) {
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