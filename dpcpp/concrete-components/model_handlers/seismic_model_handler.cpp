//
// Created by ingy on 1/26/20.
//

#include "seismic_model_handler.h"
#include "model_handler_helpers/model_handler_utils.h"
#include <bits/stdc++.h>

using namespace cl::sycl;

SeismicModelHandler::SeismicModelHandler(bool is_staggered) {
  sio = new SeIO();
  IO = new SEGYIOManager();
  this->is_staggered = is_staggered;
  grid_box = nullptr;
  parameters = nullptr;
}

void SeismicModelHandler ::GetSuitableDt(int ny, float dx, float dz, float dy,
                                         float *dt, float *coeff, int max,
                                         int half_length, float dt_relax) {
  // Calculate dt through finite difference stability equation
  float dxSquare = 1 / (dx * dx);
  float dySquare;

  if (ny != 1)
    dySquare = 1 / (dy * dy);
  else // case of 2D
    dySquare = 0.0;

  float dzSquare = 1 / (dz * dz);

  float distanceM = 1 / (sqrtf(dxSquare + dySquare + dzSquare));

  float a1 = 4; // the sum of absolute value of weights for second derivative if
                // du per dt. we use second order so the weights are (-1,2,1)
  float a2 = 0;

  for (int i = 1; i <= half_length; i++) {
    a2 += fabs(coeff[i]);
  }
  a2 *= 2.0;
  a2 += fabs(coeff[0]); // the sum of absolute values for second derivative id
                        // du per dx ( one dimension only )

  *dt = ((sqrtf(a1 / a2)) * distanceM) / max * dt_relax;
}

void SeismicModelHandler ::PreprocessModel(
    ComputationKernel *computational_kernel) {

  int nx = grid_box->window_size.window_nx;
  int nz = grid_box->window_size.window_nz;
  int ny = grid_box->window_size.window_ny;

  float dt = grid_box->dt;
  float dt2 = grid_box->dt * grid_box->dt;
  int full_nx = grid_box->grid_size.nx;
  int full_nz = grid_box->grid_size.nz;
  int full_ny = grid_box->grid_size.ny;
  unsigned long full_nx_nz =
      (unsigned long)grid_box->grid_size.nx * grid_box->grid_size.nz;

  unsigned int model_size = nx * nz * ny;
  unsigned int bytes = sizeof(float) * model_size;
  // allocating and zeroing prev, curr, and next pressure
  grid_box->pressure_current = (float *)cl::sycl::malloc_device(
      bytes + 16 * sizeof(float),
      AcousticDpcComputationParameters::device_queue->get_device(),
      AcousticDpcComputationParameters::device_queue->get_context());
  grid_box->pressure_current =
      &(grid_box->pressure_current[16 - parameters->half_length]);
  computational_kernel->FirstTouch(grid_box->pressure_current, nx, nz, ny);
  AcousticDpcComputationParameters::device_queue->submit(
      [&](handler &cgh) { cgh.memset(grid_box->pressure_current, 0, bytes); });

  if (is_staggered) {
    // for now the implementation doesn't include staggered grid box
    // TODO: add staggered implementation
  } else {
    grid_box->pressure_previous = (float *)cl::sycl::malloc_device(
        bytes + 16 * sizeof(float),
        AcousticDpcComputationParameters::device_queue->get_device(),
        AcousticDpcComputationParameters::device_queue->get_context());
    grid_box->pressure_previous =
        &(grid_box->pressure_previous[16 - parameters->half_length]);
    computational_kernel->FirstTouch(grid_box->pressure_previous, nx, nz, ny);
    AcousticDpcComputationParameters::device_queue->submit([&](handler &cgh) {
      cgh.memset(grid_box->pressure_previous, 0, bytes);
    });

    grid_box->pressure_next = grid_box->pressure_previous;
    computational_kernel->FirstTouch(grid_box->pressure_next, nx, nz, ny);
    AcousticDpcComputationParameters::device_queue->submit(
        [&](handler &cgh) { cgh.memset(grid_box->pressure_next, 0, bytes); });
  }

  if (is_staggered) {
    // for now the implementation doesn't include staggered grid box
    // TODO: add staggered implementation
  } else {
    // Preprocess the velocity model by calculating the dt2 * c2 component of
    // the wave equation.
    AcousticDpcComputationParameters::device_queue->submit(
        [&](sycl::handler &cgh) {
          auto global_range = range<3>(full_nx, full_ny, full_nz);
          auto local_range = range<3>(1, 1, 1);
          auto global_nd_range = nd_range<3>(global_range, local_range);
          float *vel_device = grid_box->velocity;
          cgh.parallel_for<class model_handler>(
              global_nd_range, [=](sycl::nd_item<3> it) {
                int x = it.get_global_id(0);
                int y = it.get_global_id(1);
                int z = it.get_global_id(2);
                float value = vel_device[y * full_nx_nz + z * full_nx + x];
                vel_device[y * full_nx_nz + z * full_nx + x] =
                    value * value * dt2;
              });
        });
    AcousticDpcComputationParameters::device_queue->wait();
  }
}

void SeismicModelHandler::SetComputationParameters(
    ComputationParameters *parameters) {
  this->parameters = parameters;
}

void SeismicModelHandler::SetGridBox(GridBox *grid_box) {
  this->grid_box = (AcousticSecondGrid *)(grid_box);
  if (this->grid_box == nullptr) {
    std::cout << "Not a compatible gridbox : "
                 "expected AcousticSecondGrid"
              << std::endl;
    exit(-1);
  }
}

GridBox *
SeismicModelHandler::ReadModel(vector<string> filenames,
                               ComputationKernel *computational_kernel) {

  string file_name = filenames[0];

  IO->ReadVelocityDataFromFile(file_name, "CSR", this->sio);

  AcousticSecondGrid *grid;
  if (is_staggered) {
    // for now the implementation doesn't include staggered grid box
    // TODO: add staggered implementation
  } else {
    grid = (AcousticSecondGrid *)mem_allocate(sizeof(AcousticSecondGrid), 1,
                                              "GridBox");
  }

  int nx, ny, nz;

  nx = grid->grid_size.nx = sio->DM.nx + 2 * parameters->boundary_length +
                            2 * parameters->half_length;
  nz = grid->grid_size.nz = sio->DM.nz + 2 * parameters->boundary_length +
                            2 * parameters->half_length;

  if (sio->DM.ny > 1) {
    ny = grid->grid_size.ny = sio->DM.ny + 2 * parameters->boundary_length +
                              2 * parameters->half_length;
  } else {
    ny = grid->grid_size.ny = 1;
  }
  uint org_nx = nx;
  uint org_nz = nz;
    // Setup window if available.
    if (parameters->use_window) {
        grid->window_size.window_start.x = 0;
        grid->window_size.window_start.z = 0;
        grid->window_size.window_start.y = 0;
        if (parameters->left_window == 0 && parameters->right_window == 0) {
            grid->window_size.window_nx = nx;
        } else {
            grid->window_size.window_nx = std::min(
                    parameters->left_window + parameters->right_window + 1 + 2 * parameters->boundary_length + 2 * parameters->half_length,
                    nx);
        }
        if (parameters->depth_window == 0) {
            grid->window_size.window_nz = nz;
        } else {
            grid->window_size.window_nz = std::min(parameters->depth_window + 2 * parameters->boundary_length + 2 * parameters->half_length, nz);
        }
        if ((parameters->front_window == 0 && parameters->back_window == 0) || ny == 1) {
            grid->window_size.window_ny = ny;
        } else {
            grid->window_size.window_ny = std::min(parameters->front_window + parameters->back_window + 1 + 2 * parameters->boundary_length + 2 * parameters->half_length, ny);
        }
    } else {
        grid->window_size.window_start.x = 0;
        grid->window_size.window_start.z = 0;
        grid->window_size.window_start.y = 0;
        grid->window_size.window_nx = nx;
        grid->window_size.window_nz = nz;
        grid->window_size.window_ny = ny;
    }
  add_helper_padding(grid, parameters);
  nx = grid->grid_size.nx;
  nz = grid->grid_size.nz;
  ny = grid->grid_size.ny;

  float dx, dy, dz, dt;

  dx = grid->cell_dimensions.dx = sio->DM.dx;
  dz = grid->cell_dimensions.dz = sio->DM.dz;
  dy = grid->cell_dimensions.dy = sio->DM.dy;

  int last = sio->Velocity.size() - 1;

  grid->reference_point.x =
      sio->Velocity.at(0).TraceMetaData.source_location_x <
              sio->Velocity.at(last).TraceMetaData.source_location_x
          ? sio->Velocity.at(0).TraceMetaData.source_location_x
          : sio->Velocity.at(last).TraceMetaData.source_location_x;

  grid->reference_point.z =
      sio->Velocity.at(0).TraceMetaData.source_location_z <
              sio->Velocity.at(last).TraceMetaData.source_location_z
          ? sio->Velocity.at(0).TraceMetaData.source_location_z
          : sio->Velocity.at(last).TraceMetaData.source_location_z;

  grid->reference_point.y =
      sio->Velocity.at(0).TraceMetaData.source_location_y <
              sio->Velocity.at(last).TraceMetaData.source_location_y
          ? sio->Velocity.at(0).TraceMetaData.source_location_y
          : sio->Velocity.at(last).TraceMetaData.source_location_y;

  std::cout << "refrence x " << grid->reference_point.x << ", refrence z "
            << grid->reference_point.z << ", refrence y "
            << grid->reference_point.y << std::endl;
  unsigned int model_size = nx * nz * ny;


  grid->velocity = (float *)cl::sycl::malloc_device(
      sizeof(float) * model_size + 16 * sizeof(float),
      AcousticDpcComputationParameters::device_queue->get_device(),
      AcousticDpcComputationParameters::device_queue->get_context());
  grid->velocity = &(grid->velocity[16 - parameters->half_length]);
  computational_kernel->FirstTouch(grid->velocity, nx, nz, ny);
  AcousticDpcComputationParameters::device_queue->submit([&](handler &cgh) {
    cgh.memset(grid->velocity, 0, sizeof(float) * model_size);
  });
  AcousticDpcComputationParameters::device_queue->wait();
    if (parameters->use_window) {
        unsigned int window_model_size = grid->window_size.window_nx *
                                         grid->window_size.window_nz * grid->window_size.window_ny;
        grid->window_velocity = (float *)cl::sycl::malloc_device(
                sizeof(float) * window_model_size + 16 * sizeof(float),
                AcousticDpcComputationParameters::device_queue->get_device(),
                AcousticDpcComputationParameters::device_queue->get_context());
        grid->window_velocity = &(grid->window_velocity[16 - parameters->half_length]);
        computational_kernel->FirstTouch(grid->window_velocity, grid->window_size.window_nx,
                                         grid->window_size.window_nz, grid->window_size.window_ny);
        AcousticDpcComputationParameters::device_queue->submit([&](handler &cgh) {
            cgh.memset(grid->window_velocity, 0, sizeof(float) * window_model_size);
        });
        AcousticDpcComputationParameters::device_queue->wait();
    } else {
        grid->window_velocity = grid->velocity;
    }
  float *velocity =
      (float *)mem_allocate(sizeof(float), model_size, "temp_model");
  memset(velocity, 0, model_size * sizeof(float));
  int offset = parameters->boundary_length + parameters->half_length;
  int offset_y =
      ny > 1 ? parameters->boundary_length + parameters->half_length : 0;
  float max = 0;
  for (unsigned int k = offset_y; k < ny - offset_y; k++) {
    for (unsigned int i = offset; i < org_nx - offset; i++) {
      for (unsigned int j = offset; j < org_nz - offset; j++) {
        float temp_velocity = velocity[k * nx * nz + j * nx + i] =
            sio->Velocity.at(i - offset).TraceData[j - offset];
        if (temp_velocity > max) {
          max = temp_velocity;
        }
      }
    }
  }
  AcousticDpcComputationParameters::device_queue->submit([&](handler &cgh) {
    cgh.memcpy(grid->velocity, velocity, sizeof(float) * model_size);
  });
  AcousticDpcComputationParameters::device_queue->wait();
  mem_free(velocity);
  if (is_staggered) {
    // for now the implementation doesn't include staggered grid box
    // TODO: add staggered implementation
  }
  printf("max velocity = %f\n", max);

  GetSuitableDt(ny, dx, dz, dy, &grid->dt,
                parameters->second_derivative_fd_coeff, max,
                parameters->half_length, parameters->dt_relax);
  free(sio);
  free(IO);
  return grid;
}

SeismicModelHandler ::~SeismicModelHandler() = default;

void SeismicModelHandler::SetupWindow() {
    if (parameters->use_window) {
        uint wnx = grid_box->window_size.window_nx;
        uint wnz = grid_box->window_size.window_nz;
        uint wny = grid_box->window_size.window_ny;
        uint nx = grid_box->grid_size.nx;
        uint nz = grid_box->grid_size.nz;
        uint ny = grid_box->grid_size.ny;
        uint sx = grid_box->window_size.window_start.x;
        uint sz = grid_box->window_size.window_start.z;
        uint sy = grid_box->window_size.window_start.y;
        uint offset = parameters->half_length + parameters->boundary_length;
        uint start_x = offset;
        uint end_x = wnx - offset;
        uint start_z = offset;
        uint end_z = wnz - offset;
        uint start_y = 0;
        uint end_y = 1;
        if (ny != 1) {
            start_y = offset;
            end_y = wny - offset;
        }
        AcousticDpcComputationParameters::device_queue->submit([&](sycl::handler &cgh) {
            auto global_range = range<3>(end_x - start_x, end_y - start_y, end_z - start_z);
            auto local_range = range<3>(1, 1, 1);
            auto global_nd_range = nd_range<3>(global_range, local_range);
            float *vel = grid_box->velocity;
            float *w_vel = grid_box->window_velocity;
            cgh.parallel_for<class model_handler>(global_nd_range, [=](sycl::nd_item<3> it) {
                int x = it.get_global_id(0) + start_x;
                int y = it.get_global_id(1) + start_y;
                int z = it.get_global_id(2) + start_z;
                uint offset_window = y * wnx * wnz + z * wnx + x;
                uint offset_full = (y + sy) * nx * nz + (z + sz) * nx + x + sx;
                w_vel[offset_window] = vel[offset_full];
            });
        });
        AcousticDpcComputationParameters::device_queue->wait();
    }
}
