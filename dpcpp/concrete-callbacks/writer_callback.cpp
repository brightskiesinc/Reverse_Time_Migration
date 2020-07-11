#include "writer_callback.h"
#include <concrete-components/data_units/acoustic_dpc_computation_parameters.h>
#include <concrete-components/data_units/acoustic_second_grid.h>
#include <sys/stat.h>

#define CAT_STR_TO_CHR(a, b) ((char *)string(a + b).c_str())

WriterCallback::WriterCallback(uint show_each, bool write_velocity, bool write_forward,
                       bool write_backward, bool write_reverse,
                       bool write_migration, bool write_re_extended_velocity,
                       bool write_single_shot_correlation,
                       bool write_each_stacked_shot, bool write_traces_raw,
                       bool write_traces_preprocessed, string write_path, string folder_name) {
  this->show_each = show_each;
  this->shot_num = 0;
  this->write_velocity = write_velocity;
  this->write_forward = write_forward;
  this->write_backward = write_backward;
  this->write_reverse = write_reverse;
  this->write_migration = write_migration;
  this->write_re_extended_velocity = write_re_extended_velocity;
  this->write_single_shot_correlation = write_single_shot_correlation;
  this->write_each_stacked_shot = write_each_stacked_shot;
  this->write_traces_raw = write_traces_raw;
  this->write_traces_preprocessed = write_traces_preprocessed;
  this->write_path = write_path;
  mkdir(write_path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  this->write_path = this->write_path + "/" + folder_name;
  mkdir(this->write_path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  if (write_re_extended_velocity) {
      mkdir(CAT_STR_TO_CHR(this->write_path, "/velocities"),
            S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  }
  if (write_single_shot_correlation) {
      mkdir(CAT_STR_TO_CHR(this->write_path, "/shots"),
            S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  }
  if (write_each_stacked_shot) {
      mkdir(CAT_STR_TO_CHR(this->write_path, "/stacked_shots"),
            S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  }
  if (write_forward) {
      mkdir(CAT_STR_TO_CHR(this->write_path, "/forward"),
            S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  }
  if (write_reverse) {
      mkdir(CAT_STR_TO_CHR(this->write_path, "/reverse"),
            S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  }
  if (write_backward) {
      mkdir(CAT_STR_TO_CHR(this->write_path, "/backward"),
            S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  }
  if (write_traces_raw) {
      mkdir(CAT_STR_TO_CHR(this->write_path, "/traces_raw"),
            S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  }
  if (write_traces_preprocessed) {
      mkdir(CAT_STR_TO_CHR(this->write_path, "/traces"),
            S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  }
}

float *obtain_unpadded(float *ptr, GridBox *box, bool window) {
  uint nz = box->grid_size.nz;
  uint nx = box->grid_size.nx;
  auto grid = (AcousticSecondGrid *)box;
  uint org_nx = grid->full_original_dimensions.nx;
  uint org_nz = grid->full_original_dimensions.nz;
  if (window) {
      nz = box->window_size.window_nz;
      nx = box->window_size.window_nx;
      org_nx = grid->original_dimensions.nx;
      org_nz = grid->original_dimensions.nz;
  }
  auto unpadded_temp = new float[org_nx * org_nz];
  for (int i = 0; i < org_nz; i++) {
    for (int j = 0; j < org_nx; j++) {
      unpadded_temp[i * org_nx + j] = ptr[i * nx + j];
    }
  }
  return unpadded_temp;
}

float *obtain_unpadded_host_accessor(float *ptr, GridBox *box, bool window) {
  uint nz = box->grid_size.nz;
  uint nx = box->grid_size.nx;
  uint ny = box->grid_size.ny;
  if (window) {
      nz = box->window_size.window_nz;
      nx = box->window_size.window_nx;
      ny = box->window_size.window_ny;
  }
  uint nx_nz = nx * nz;
  auto temp = new float[nx_nz * ny];
  auto grid = (AcousticSecondGrid *)box;
  AcousticDpcComputationParameters::device_queue->wait();
  AcousticDpcComputationParameters::device_queue->submit(
      [&](sycl::handler &cgh) {
        cgh.memcpy(temp, ptr, nx_nz * ny * sizeof(float));
      });
  AcousticDpcComputationParameters::device_queue->wait();
  auto unpadded_temp = obtain_unpadded(temp, box, window);
  delete[] temp;
  return unpadded_temp;
}

void WriterCallback::BeforeInitialization(ComputationParameters *parameters) {}
void WriterCallback::AfterInitialization(GridBox *box) {

  if (write_velocity) {

    auto grid = (AcousticSecondGrid *)box;
    float *temp = obtain_unpadded_host_accessor(box->velocity, box, false);

    uint nz = grid->full_original_dimensions.nz;
    uint nx = grid->full_original_dimensions.nx;
    uint ny = grid->full_original_dimensions.ny;
    uint nt = box->nt;
    float dx = box->cell_dimensions.dx;
    float dy = box->cell_dimensions.dy;
    float dz = box->cell_dimensions.dz;
    float dt = box->dt;

    WriteResult(nx, nz, nt, ny, dx, dz, dt, dy, temp,
              CAT_STR_TO_CHR(write_path, "/velocity" + this->GetExtension()), false);

    delete[] temp;
  }
}

void WriterCallback::BeforeShotPreprocessing(Traces *traces) {

  if (write_traces_raw) {

    uint nz = traces->sample_nt;
    uint nx = traces->trace_size_per_timestep;
    uint ny = traces->num_receivers_in_y;

    uint nt = traces->sample_nt;
    float dt = traces->sample_dt;

    float dx = 0.0;
    float dy = 0.0;
    float dz = 0.0;

    uint nx_nz = nx * nz;

    WriteResult(nx, nz, nt, ny, dx, dz, dt, dy, traces->traces,
              (char *)string(write_path + "/traces_raw/trace_" +
                             to_string(shot_num) + this->GetExtension())
                  .c_str(),
              true);
  }
}

void WriterCallback::AfterShotPreprocessing(Traces *traces) {

  if (write_traces_preprocessed) {

    uint nz = traces->sample_nt;
    uint nx = traces->trace_size_per_timestep;
    uint ny = traces->num_receivers_in_y;

    uint nt = traces->sample_nt;
    float dt = traces->sample_dt;

    float dx = 0.0;
    float dy = 0.0;
    float dz = 0.0;

    uint nx_nz = nx * nz;

    WriteResult(nx, nz, nt, ny, dx, dz, dt, dy, traces->traces,
              (char *)string(write_path + "/traces/trace_" +
                             to_string(shot_num) + this->GetExtension())
                  .c_str(),
              true);
  }
}

void WriterCallback::BeforeForwardPropagation(GridBox *box) {

  if (write_re_extended_velocity) {

    auto grid = (AcousticSecondGrid *)box;
    float *temp = obtain_unpadded_host_accessor(box->window_velocity, box, true);

    uint nz = grid->original_dimensions.nz;
    uint nx = grid->original_dimensions.nx;
    uint ny = grid->original_dimensions.ny;
    uint nt = box->nt;
    float dx = box->cell_dimensions.dx;
    float dy = box->cell_dimensions.dy;
    float dz = box->cell_dimensions.dz;
    float dt = box->dt;

    WriteResult(nx, nz, nt, ny, dx, dz, dt, dy, temp,
              (char *)string(write_path + "/velocities/velocity_" +
                             to_string(shot_num) + this->GetExtension())
                  .c_str(),
              false);

    delete[] temp;
  }
}

void WriterCallback::AfterForwardStep(GridBox *box, uint time_step) {

  if (write_forward && time_step % show_each == 0) {

    auto grid = (AcousticSecondGrid *)box;
    float *temp = obtain_unpadded_host_accessor(box->pressure_current, box, true);

    uint nz = grid->original_dimensions.nz;
    uint nx = grid->original_dimensions.nx;
    uint ny = grid->original_dimensions.ny;
    uint nt = box->nt;
    float dx = box->cell_dimensions.dx;
    float dy = box->cell_dimensions.dy;
    float dz = box->cell_dimensions.dz;
    float dt = box->dt;

    WriteResult(nx, nz, nt, ny, dx, dz, dt, dy, temp,
              (char *)string(write_path + "/forward/forward_" +
                             to_string(time_step) + this->GetExtension())
                  .c_str(),
              false);

    delete[] temp;
  }
}

void WriterCallback::BeforeBackwardPropagation(GridBox *box) {

  if (write_re_extended_velocity) {

    auto grid = (AcousticSecondGrid *)box;
    float *temp = obtain_unpadded_host_accessor(box->window_velocity, box, true);

    uint nz = grid->original_dimensions.nz;
    uint nx = grid->original_dimensions.nx;
    uint ny = grid->original_dimensions.ny;
    uint nt = box->nt;
    float dx = box->cell_dimensions.dx;
    float dy = box->cell_dimensions.dy;
    float dz = box->cell_dimensions.dz;
    float dt = box->dt;

    WriteResult(nx, nz, nt, ny, dx, dz, dt, dy, temp,
              (char *)string(write_path + "/velocities/velocity_backward_" +
                             to_string(shot_num) + this->GetExtension())
                  .c_str(),
              false);
    delete[] temp;
  }
}

void WriterCallback::AfterBackwardStep(GridBox *box, uint time_step) {

  if (write_backward && time_step % show_each == 0) {

    auto grid = (AcousticSecondGrid *)box;
    float *temp = obtain_unpadded_host_accessor(box->pressure_current, box, true);

    uint nz = grid->original_dimensions.nz;
    uint nx = grid->original_dimensions.nx;
    uint ny = grid->original_dimensions.ny;
    uint nt = box->nt;
    float dx = box->cell_dimensions.dx;
    float dy = box->cell_dimensions.dy;
    float dz = box->cell_dimensions.dz;
    float dt = box->dt;

    WriteResult(nx, nz, nt, ny, dx, dz, dt, dy, temp,
              (char *)string(write_path + "/backward/backward_" +
                             to_string(time_step) + this->GetExtension())
                  .c_str(),
              false);

    delete[] temp;
  }
}

void WriterCallback::AfterFetchStep(GridBox *forward_collector_box,
                                uint time_step) {

  if (write_reverse && time_step % show_each == 0) {

    auto grid = (AcousticSecondGrid *)forward_collector_box;
    float *temp = obtain_unpadded_host_accessor(
        forward_collector_box->pressure_current, forward_collector_box, true);

    uint nz = grid->original_dimensions.nz;
    uint nx = grid->original_dimensions.nx;
    uint ny = grid->original_dimensions.ny;
    uint nt = grid->nt;
    float dx = forward_collector_box->cell_dimensions.dx;
    float dy = forward_collector_box->cell_dimensions.dy;
    float dz = forward_collector_box->cell_dimensions.dz;
    float dt = forward_collector_box->dt;

    WriteResult(nx, nz, nt, ny, dx, dz, dt, dy, temp,
              (char *)string(write_path + "/reverse/reverse_" +
                             to_string(time_step) + this->GetExtension())
                  .c_str(),
              false);

    delete[] temp;
  }
}

void WriterCallback::BeforeShotStacking(float *shot_correlation,
                                    GridBox *meta_data) {

  if (write_single_shot_correlation) {

    auto grid = (AcousticSecondGrid *)meta_data;
    auto temp = obtain_unpadded(shot_correlation, meta_data, true);

    uint nz = grid->original_dimensions.nz;
    uint nx = grid->original_dimensions.nx;
    uint ny = grid->original_dimensions.ny;
    uint nt = grid->nt;
    float dx = meta_data->cell_dimensions.dx;
    float dy = meta_data->cell_dimensions.dy;
    float dz = meta_data->cell_dimensions.dz;
    float dt = meta_data->dt;

    WriteResult(nx, nz, nt, ny, dx, dz, dt, dy, temp,
              (char *)string(write_path + "/shots/correlation_" +
                             to_string(shot_num) + this->GetExtension())
                  .c_str(),
              false);

    delete[] temp;
  }
}

void WriterCallback::AfterShotStacking(float *stacked_shot_correlation,
                                   GridBox *meta_data) {

  if (write_each_stacked_shot) {

    auto grid = (AcousticSecondGrid *)meta_data;
    auto temp = obtain_unpadded(stacked_shot_correlation, meta_data, false);

    uint nz = grid->full_original_dimensions.nz;
    uint nx = grid->full_original_dimensions.nx;
    uint ny = grid->full_original_dimensions.ny;
    uint nt = grid->nt;
    float dx = meta_data->cell_dimensions.dx;
    float dy = meta_data->cell_dimensions.dy;
    float dz = meta_data->cell_dimensions.dz;
    float dt = meta_data->dt;

    WriteResult(nx, nz, nt, ny, dx, dz, dt, dy, temp,
              (char *)string(write_path +
                             "/stacked_shots/stacked_correlation_" +
                             to_string(shot_num) + this->GetExtension())
                  .c_str(),
              false);

    delete[] temp;
  }

  this->shot_num++;
}

void WriterCallback::AfterMigration(float *stacked_shot_correlation,
                                GridBox *meta_data) {

  if (write_migration) {

    auto grid = (AcousticSecondGrid *)meta_data;
    auto temp = obtain_unpadded(stacked_shot_correlation, meta_data, false);

    uint nz = grid->full_original_dimensions.nz;
    uint nx = grid->full_original_dimensions.nx;
    uint ny = grid->full_original_dimensions.ny;
    uint nt = grid->nt;
    float dx = meta_data->cell_dimensions.dx;
    float dy = meta_data->cell_dimensions.dy;
    float dz = meta_data->cell_dimensions.dz;
    float dt = meta_data->dt;

    WriteResult(nx, nz, nt, ny, dx, dz, dt, dy, temp,
              CAT_STR_TO_CHR(write_path, "/migration" + this->GetExtension()), false);

    delete[] temp;
  }
}