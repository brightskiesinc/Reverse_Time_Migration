#include "writer_callback.h"
#include <skeleton/base/datatypes.h>
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

void WriterCallback::BeforeInitialization(ComputationParameters *parameters) {}
void WriterCallback::AfterInitialization(GridBox *box) {

    if (write_velocity) {
        uint nz = box->grid_size.nz;
        uint nx = box->grid_size.nx;
        uint ny = box->grid_size.ny;
        uint nt = box->nt;
        float dx = box->cell_dimensions.dx;
        float dy = box->cell_dimensions.dy;
        float dz = box->cell_dimensions.dz;
        float dt = box->dt;

        uint nx_nz = nx * nz;

        WriteResult(nx, nz, nt, ny, dx, dz, dt, dy, box->velocity,
                  CAT_STR_TO_CHR(write_path, "/velocity" + this->GetExtension()), false);
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

        uint nz = box->grid_size.nz;
        uint nx = box->grid_size.nx;
        uint ny = box->grid_size.ny;
        uint nt = box->nt;
        uint nx_nz = nx * nz;

        float dx = box->cell_dimensions.dx;
        float dy = box->cell_dimensions.dy;
        float dz = box->cell_dimensions.dz;
        float dt = box->dt;

        WriteResult(nx, nz, nt, ny, dx, dz, dt, dy, box->velocity,
                  (char *)string(write_path + "/velocities/velocity_" +
                                 to_string(shot_num) + this->GetExtension())
                          .c_str(),
                  false);
    }
}

void WriterCallback::AfterForwardStep(GridBox *box, uint time_step) {

    if (write_forward && time_step % show_each == 0) {

        uint nz = box->window_size.window_nz;
        uint nx = box->window_size.window_nx;
        uint nx_nz = nx * nz;
        uint ny = box->window_size.window_ny;
        uint nt = box->nt;

        float dx = box->cell_dimensions.dx;
        float dy = box->cell_dimensions.dy;
        float dz = box->cell_dimensions.dz;
        float dt = box->dt;

        WriteResult(nx, nz, nt, ny, dx, dz, dt, dy,
                  box->pressure_current,
                  (char *)string(write_path + "/forward/forward_" +
                                 to_string(time_step) + this->GetExtension())
                          .c_str(),
                  false);
    }
}

void WriterCallback::BeforeBackwardPropagation(GridBox *box) {

    if (write_re_extended_velocity) {

        uint nz = box->grid_size.nz;
        uint nx = box->grid_size.nx;
        uint ny = box->grid_size.ny;
        uint nt = box->nt;
        uint nx_nz = nx * nz;

        float dx = box->cell_dimensions.dx;
        float dy = box->cell_dimensions.dy;
        float dz = box->cell_dimensions.dz;
        float dt = box->dt;

        WriteResult(nx, nz, nt, ny, dx, dz, dt, dy, box->velocity,
                  (char *)string(write_path + "/velocities/velocity_backward_" +
                                 to_string(shot_num) + this->GetExtension())
                          .c_str(),
                  false);
    }
}

void WriterCallback::AfterBackwardStep(GridBox *box, uint time_step) {

    if (write_backward && time_step % show_each == 0) {

        uint nz = box->window_size.window_nz;
        uint nx = box->window_size.window_nx;
        uint nx_nz = nx * nz;
        uint ny = box->window_size.window_ny;
        uint nt = box->nt;

        float dx = box->cell_dimensions.dx;
        float dy = box->cell_dimensions.dy;
        float dz = box->cell_dimensions.dz;
        float dt = box->dt;

        WriteResult(nx, nz, nt, ny, dx, dz, dt, dy,
                  box->pressure_current,
                  (char *)string(write_path + "/backward/backward_" +
                                 to_string(time_step) + this->GetExtension())
                          .c_str(),
                  false);
    }
}

void WriterCallback::AfterFetchStep(GridBox *forward_collector_box,
                                uint time_step) {

    if (write_reverse && time_step % show_each == 0) {

        uint nz = forward_collector_box->window_size.window_nz;
        uint nx = forward_collector_box->window_size.window_nx;
        uint nx_nz = nx * nz;
        uint ny = forward_collector_box->window_size.window_ny;
        uint nt = forward_collector_box->nt;

        float dx = forward_collector_box->cell_dimensions.dx;
        float dy = forward_collector_box->cell_dimensions.dy;
        float dz = forward_collector_box->cell_dimensions.dz;
        float dt = forward_collector_box->dt;

        WriteResult(nx, nz, nt, ny, dx, dz, dt, dy,
                  forward_collector_box->pressure_current,
                  (char *)string(write_path + "/reverse/reverse_" +
                                 to_string(time_step) + this->GetExtension())
                          .c_str(),
                  false);
    }
}

void WriterCallback::BeforeShotStacking(float *shot_correlation,
                                    GridBox *meta_data) {

    if (write_single_shot_correlation) {

        uint nz = meta_data->grid_size.nz;
        uint nx = meta_data->grid_size.nx;
        uint nx_nz = nx * nz;
        uint ny = meta_data->grid_size.ny;
        uint nt = meta_data->nt;
        float dx = meta_data->cell_dimensions.dx;
        float dy = meta_data->cell_dimensions.dy;
        float dz = meta_data->cell_dimensions.dz;
        float dt = meta_data->dt;

        WriteResult(nx, nz, nt, ny, dx, dz, dt, dy,
                  shot_correlation,
                  (char *)string(write_path + "/shots/correlation_" +
                                 to_string(shot_num) + this->GetExtension())
                          .c_str(),
                  false);
    }
}

void WriterCallback::AfterShotStacking(float *stacked_shot_correlation,
                                   GridBox *meta_data) {

    if (write_each_stacked_shot) {

        uint nz = meta_data->grid_size.nz;
        uint nx = meta_data->grid_size.nx;
        uint nx_nz = nx * nz;
        uint ny = meta_data->grid_size.ny;
        uint nt = meta_data->nt;
        float dx = meta_data->cell_dimensions.dx;
        float dy = meta_data->cell_dimensions.dy;
        float dz = meta_data->cell_dimensions.dz;
        float dt = meta_data->dt;

        WriteResult(nx, nz, nt, ny, dx, dz, dt, dy,
                  stacked_shot_correlation,
                  (char *)string(write_path +
                                 "/stacked_shots/stacked_correlation_" +
                                 to_string(shot_num) + this->GetExtension())
                          .c_str(),
                  false);
    }

    this->shot_num++;
}

void WriterCallback::AfterMigration(float *stacked_shot_correlation,
                                GridBox *meta_data) {

    if (write_migration) {

        uint nz = meta_data->grid_size.nz;
        uint nx = meta_data->grid_size.nx;
        uint nx_nz = nx * nz;
        uint ny = meta_data->grid_size.ny;
        uint nt = meta_data->nt;
        float dx = meta_data->cell_dimensions.dx;
        float dy = meta_data->cell_dimensions.dy;
        float dz = meta_data->cell_dimensions.dz;
        float dt = meta_data->dt;

        WriteResult(nx, nz, nt, ny, dx, dz, dt, dy,
                  stacked_shot_correlation,
                  CAT_STR_TO_CHR(write_path, "/migration" + this->GetExtension()), false);
    }
}