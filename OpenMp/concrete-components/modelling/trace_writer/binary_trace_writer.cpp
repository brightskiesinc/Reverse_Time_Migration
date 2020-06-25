//
// Created by amrnasr on 13/11/2019.
//

#include "binary_trace_writer.h"

BinaryTraceWriter::BinaryTraceWriter() { this->output_file = nullptr; }

BinaryTraceWriter::~BinaryTraceWriter() {
  if (this->output_file != nullptr) {
    delete this->output_file;
  }
}

void BinaryTraceWriter::SetComputationParameters(
    ComputationParameters *parameters) {
  this->parameters = (AcousticOmpComputationParameters *)(parameters);
  if (this->parameters == nullptr) {
    std::cout << "Not a compatible computation parameters : "
                 "expected AcousticOmpComputationParameters"
              << std::endl;
    exit(-1);
  }
}

void BinaryTraceWriter::SetGridBox(GridBox *grid_box) { this->grid = grid_box; }

Point3D LocalizePoint(Point3D point, bool is_2D, uint half_length,
                      uint bound_length) {
  Point3D copy;
  copy.x = point.x - half_length - bound_length;
  copy.z = point.z - half_length - bound_length;
  if (!is_2D) {
    copy.y = point.y - half_length - bound_length;
  } else {
    copy.y = point.y;
  }
  return copy;
}
void BinaryTraceWriter::InitializeWriter(
    ModellingConfiguration *modelling_config, string output_filename) {
  this->output_file = new ofstream(output_filename, ios::out | ios::binary);
  bool is_2D = grid->grid_size.ny == 1;
  Point3D local_source =
      LocalizePoint(modelling_config->source_point, is_2D,
                    parameters->half_length, parameters->boundary_length);
  Point3D local_start =
      LocalizePoint(modelling_config->receivers_start, is_2D,
                    parameters->half_length, parameters->boundary_length);
  Point3D local_end =
      LocalizePoint(modelling_config->receivers_end, is_2D,
                    parameters->half_length, parameters->boundary_length);
  output_file->write((char *)&local_source, sizeof(local_source));
  output_file->write((char *)&local_start, sizeof(local_start));
  output_file->write((char *)&modelling_config->receivers_increment,
                     sizeof(modelling_config->receivers_increment));
  output_file->write((char *)&local_end, sizeof(local_end));
  output_file->write((char *)&modelling_config->total_time,
                     sizeof(modelling_config->total_time));
  output_file->write((char *)&grid->dt, sizeof(grid->dt));
  r_start = modelling_config->receivers_start;
  r_end = modelling_config->receivers_end;
  r_inc = modelling_config->receivers_increment;
}

#include <iostream>

void BinaryTraceWriter::RecordTrace() {
  int x_inc = r_inc.x == 0 ? 1 : r_inc.x;
  int y_inc = r_inc.y == 0 ? 1 : r_inc.y;
  int z_inc = r_inc.z == 0 ? 1 : r_inc.z;
  int wnx = grid->window_size.window_nx;
  int wnz_wnx = grid->window_size.window_nz * wnx;
  for (int iz = r_start.z; iz < r_end.z; iz += z_inc) {
    for (int iy = r_start.y; iy < r_end.y; iy += y_inc) {
      for (int ix = r_start.x; ix < r_end.x; ix += x_inc) {
        int offset = iy * wnz_wnx + iz * wnx + ix;
        output_file->write((char *)&grid->pressure_current[offset],
                           sizeof(grid->pressure_current[offset]));
      }
    }
  }
}
