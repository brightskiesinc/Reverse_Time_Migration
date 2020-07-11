//
// Created by amr on 18/01/2020.
//

#include "norm_writer.h"
#include <cmath>
#include <concrete-components/data_units/acoustic_dpc_computation_parameters.h>
#include <sys/stat.h>

#define CAT_STR(a, b) (a + b)

NormWriter::NormWriter(uint show_each, bool write_forward, bool write_backward,
                       bool write_reverse, string write_path) {
  this->show_each = show_each;
  this->write_forward = write_forward;
  this->write_backward = write_backward;
  this->write_reverse = write_reverse;
  this->write_path = write_path;
  mkdir(write_path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  this->write_path = this->write_path + "/norm";
  mkdir(this->write_path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  if (this->write_forward) {
    forward_norm_stream = new ofstream(this->write_path + "/forward_norm.tsv");
  }
  if (this->write_reverse) {
    reverse_norm_stream = new ofstream(this->write_path + "/reverse_norm.tsv");
  }
  if (this->write_backward) {
    backward_norm_stream =
        new ofstream(this->write_path + "/backward_norm.tsv");
  }
}

float NormWriter::GetNorm(float *mat, uint nx, uint nz, uint ny) {
  float sum = 0;
  uint nx_nz = nx * nz;
#pragma omp parallel for collapse(2) reduction(+ : sum)
  for (int iy = 0; iy < ny; iy++) {
    for (int iz = 0; iz < nz; iz++) {
#pragma ivdep
      for (int ix = 0; ix < nx; ix++) {
        float value = mat[iy * nx_nz + nx * iz + ix];
        sum = sum + (value * value);
      }
    }
  }
  return sqrtf(sum);
}

float *obtain_host_accessor_norm(float *ptr, GridBox *box) {
  uint nz = box->window_size.window_nz;
  uint nx = box->window_size.window_nx;
  uint nx_nz = nx * nz;
  uint ny = box->window_size.window_ny;
  auto temp = new float[nx_nz * ny];
  AcousticDpcComputationParameters::device_queue->wait();
  AcousticDpcComputationParameters::device_queue->submit(
      [&](sycl::handler &cgh) {
        cgh.memcpy(temp, ptr, nx_nz * ny * sizeof(float));
      });
  AcousticDpcComputationParameters::device_queue->wait();
  return temp;
}

void NormWriter::BeforeInitialization(ComputationParameters *parameters) {}

void NormWriter::AfterInitialization(GridBox *box) {}

void NormWriter::BeforeShotPreprocessing(Traces *traces) {}

void NormWriter::AfterShotPreprocessing(Traces *traces) {}

void NormWriter::BeforeForwardPropagation(GridBox *box) {}

void NormWriter::AfterForwardStep(GridBox *box, uint time_step) {
  if (write_forward && time_step % show_each == 0) {
    uint nz = box->window_size.window_nz;
    uint nx = box->window_size.window_nx;
    uint ny = box->window_size.window_ny;
    float *temp = obtain_host_accessor_norm(box->pressure_current, box);
    float norm = GetNorm(temp, nx, nz, ny);
    (*this->forward_norm_stream) << time_step << "\t" << norm << endl;
    delete[] temp;
  }
}

void NormWriter::BeforeBackwardPropagation(GridBox *box) {}

void NormWriter::AfterBackwardStep(GridBox *box, uint time_step) {
  if (write_backward && time_step % show_each == 0) {
    uint nz = box->window_size.window_nz;
    uint nx = box->window_size.window_nx;
    uint ny = box->window_size.window_ny;
    float *temp = obtain_host_accessor_norm(box->pressure_current, box);
    float norm = GetNorm(temp, nx, nz, ny);
    (*this->backward_norm_stream) << time_step << "\t" << norm << endl;
    delete[] temp;
  }
}

void NormWriter::AfterFetchStep(GridBox *forward_collector_box,
                                uint time_step) {
  if (write_reverse && time_step % show_each == 0) {
    uint nz = forward_collector_box->window_size.window_nz;
    uint nx = forward_collector_box->window_size.window_nx;
    uint ny = forward_collector_box->window_size.window_ny;
    float *temp = obtain_host_accessor_norm(
        forward_collector_box->pressure_current, forward_collector_box);
    float norm = GetNorm(temp, nx, nz, ny);
    (*this->reverse_norm_stream) << time_step << "\t" << norm << endl;
    delete[] temp;
  }
}

void NormWriter::BeforeShotStacking(float *shot_correlation,
                                    GridBox *meta_data) {}

void NormWriter::AfterShotStacking(float *stacked_shot_correlation,
                                   GridBox *meta_data) {}

void NormWriter::AfterMigration(float *stacked_shot_correlation,
                                GridBox *meta_data) {}
