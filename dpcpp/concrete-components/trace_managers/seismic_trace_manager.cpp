//
// Created by ingy on 1/28/20.
//

#include "seismic_trace_manager.h"
#include <cmath>
#include <seismic-io-framework/datatypes.h>
#include <skeleton/helpers/dout/dout.h>
#include <skeleton/helpers/memory_allocation/memory_allocator.h>
#include <utility>
#include <read_utils.h>

using namespace cl::sycl;

SeismicTraceManager::SeismicTraceManager() {
  traces = new Traces();
  traces->traces = nullptr;
  sio = nullptr;
  IO = new SEGYIOManager();
  grid = nullptr;
  parameters = nullptr;
  ptr_traces = nullptr;
  x_positions = nullptr;
  y_positions = nullptr;
  total_time = 0;
}

SeismicTraceManager::~SeismicTraceManager() {
  if (traces->traces != nullptr) {
    mem_free(traces->traces);
    cl::sycl::free(ptr_traces, AcousticDpcComputationParameters::device_queue->get_context());
    cl::sycl::free(x_positions, AcousticDpcComputationParameters::device_queue->get_context());
    cl::sycl::free(y_positions, AcousticDpcComputationParameters::device_queue->get_context());
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
    cl::sycl::free(ptr_traces, AcousticDpcComputationParameters::device_queue->get_context());
    cl::sycl::free(x_positions, AcousticDpcComputationParameters::device_queue->get_context());
    cl::sycl::free(y_positions, AcousticDpcComputationParameters::device_queue->get_context());
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
  uint *temp_x_position, *temp_y_position;
  ParseSeioToTraces(sio, &this->source_point, this->traces, &temp_x_position,
          &temp_y_position, this->grid, this->parameters, &this->total_time);
  unsigned long sizeTotal = traces->trace_size_per_timestep * traces->sample_nt;
  ptr_traces = (float *)cl::sycl::malloc_device(
          sizeof(float) * sizeTotal,
          AcousticDpcComputationParameters::device_queue->get_device(),
          AcousticDpcComputationParameters::device_queue->get_context());
  x_positions = (uint *)cl::sycl::malloc_device(
          sizeof(uint) * traces->trace_size_per_timestep,
          AcousticDpcComputationParameters::device_queue->get_device(),
          AcousticDpcComputationParameters::device_queue->get_context());
  y_positions = (uint *)cl::sycl::malloc_device(
          sizeof(uint) * traces->trace_size_per_timestep,
          AcousticDpcComputationParameters::device_queue->get_device(),
          AcousticDpcComputationParameters::device_queue->get_context());
  AcousticDpcComputationParameters::device_queue->submit([&](handler &cgh) {
      cgh.memcpy(ptr_traces, traces->traces, sizeof(float) * sizeTotal);
  });
  AcousticDpcComputationParameters::device_queue->submit([&](handler &cgh) {
      cgh.memcpy(x_positions, temp_x_position, sizeof(uint) * traces->trace_size_per_timestep);
  });
  AcousticDpcComputationParameters::device_queue->submit([&](handler &cgh) {
      cgh.memcpy(y_positions, temp_y_position, sizeof(uint) * traces->trace_size_per_timestep);
  });
  AcousticDpcComputationParameters::device_queue->wait();
  mem_free(temp_x_position);
  mem_free(temp_y_position);
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

    uint trace_step = uint(current_time / traces->sample_dt);
    trace_step = std::min(trace_step, traces->sample_nt - 1);

    AcousticDpcComputationParameters::device_queue->submit([&](handler &cgh) {
        auto global_range = range<1>(trace_size);
        auto local_range = range<1>(1);
        auto global_nd_range = nd_range<1>(global_range, local_range);

        float *current = grid->pressure_current;
        float *d_traces = ptr_traces;
        uint *x_pos = x_positions;
        uint *y_pos = y_positions;
        cgh.parallel_for<class trace_manager>(global_nd_range, [=](nd_item<1> it) {
            int i = it.get_global_id(0);
            int offset = y_pos[i] * wnz_wnx + std_offset + x_pos[i];
            current[offset] += d_traces[(trace_step)*trace_size + i];
        });
    });
    AcousticDpcComputationParameters::device_queue->wait();
}

Traces *SeismicTraceManager::GetTraces() { return traces; }

void SeismicTraceManager::SetComputationParameters(
    ComputationParameters *parameters) {
  this->parameters = (AcousticDpcComputationParameters *)(parameters);
  if (this->parameters == nullptr) {
    std::cout << "Not a compatible computation parameters : "
                 "expected AcousticDpcComputationParameters"
              << std::endl;
    exit(-1);
  }
}

void SeismicTraceManager::SetGridBox(GridBox *grid_box) {
  this->grid = (AcousticSecondGrid *)(grid_box);
  if (this->grid == nullptr) {
    std::cout << "Not a compatible gridbox : "
                 "expected AcousticSecondGrid"
              << std::endl;
    exit(-1);
  }
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
