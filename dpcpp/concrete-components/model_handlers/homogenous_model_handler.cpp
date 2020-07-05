#include "homogenous_model_handler.h"
#include "model_handler_helpers/model_handler_utils.h"
#include <cmath>
#include <concrete-components/data_units/acoustic_second_grid.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

GridBox *
HomogenousModelHandler ::ReadModel(vector<string> filenames,
                                   ComputationKernel *computational_kernel) {
  string file_name = filenames[0];
  ifstream inFile(file_name);
  string temp_line;
  string temp_s;
  float temp;
  vector<vector<float>> val;
  vector<string> v;
  // Read each line in the text file and insert in vector V
  while (getline(inFile, temp_line, '|')) {
    v.push_back(temp_line);
  }
  // Loop on vector v and insert nx,ny,nz,dx,dy,dz in val[0]
  // velocity, start and end of each layer in val[1];
  // start and end of window in val[2]
  for (int i = 0; i < v.size(); i++) {
    stringstream ss(v[i]);
    string item;
    vector<float> items;
    while (getline(ss, item, ',')) {
      items.push_back(atof(item.c_str()));
    }
    val.push_back(items);
  }
  // setting nx,ny,nz and adding Bound length
  AcousticSecondGrid *grid = (AcousticSecondGrid *)mem_allocate(
      sizeof(AcousticSecondGrid), 1, "GridBox");

  int nx, ny, nz;
  nx = grid->grid_size.nx =
      val[0][0] + 2 * parameters->boundary_length + 2 * parameters->half_length;
  nz = grid->grid_size.nz =
      val[0][1] + 2 * parameters->boundary_length + 2 * parameters->half_length;
  if (val[0][2] > 1) {
    ny = grid->grid_size.ny = val[0][2] + 2 * parameters->boundary_length +
                              2 * parameters->half_length;
  } else {
    ny = grid->grid_size.ny = 1;
  }

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

    dx = grid->cell_dimensions.dx = val[0][3];
    dz = grid->cell_dimensions.dz = val[0][4];
    dy = grid->cell_dimensions.dy = val[0][5];
    // Calculate Model Size
    unsigned int model_size = nx * nz * ny;
  // Allocation and Zeroing the velocity model
  float *velocity =
      (float *)mem_allocate(sizeof(float), model_size, "temp_model");
  memset(velocity, 0, model_size * sizeof(float));
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
  // extracting Velocity and size of each layer in terms of start(x,y,z) and
  // end(x,y,z)
  float vel;
  int vsx, vsy, vsz, vex, vey, vez;
  float max = 0;
  for (int i = 0; i < val[1].size(); i += 7) {
    vel = val[1][i];
    if (vel > max) {
      max = vel;
    }
    // starting filling the velocity with required value value after shifting
    // the padding of boundaries and half_length
    int offset = parameters->boundary_length + parameters->half_length;
    vsx = (int)val[1][i + 1] + offset;
    vsz = (int)val[1][i + 2] + offset;
    vsy = (int)val[1][i + 3];
    vex = (int)val[1][i + 4] + offset;
    vez = (int)val[1][i + 5] + offset;
    vey = (int)val[1][i + 6];

    if (ny > 1) {
      vsy = (int)vsy + offset;
      vey = (int)vey + offset;
    }
    if (vsx < 0 || vsy < 0 || vsz < 0) {
      cout << "Error at starting index values (x,y,z) (" << vsx << "," << vsy
           << "," << vsz << ")"
           << "\n";
      continue;
    }
    if (vex > nx || vey > ny || vez > nz) {
      cout << "Error at ending index values (x,y,z) (" << vex << "," << vey
           << "," << vez << ")"
           << "\n";
      continue;
    }

    // initialize the layer with the extracted velocity value
    for (int y = vsy; y < vey; y++) {
      for (int z = vsz; z < vez; z++) {
        for (int x = vsx; x < vex; x++) {
          velocity[y * nx * nz + z * nx + x] = vel;
        }
      }
    }
  }
  AcousticDpcComputationParameters::device_queue->submit([&](handler &cgh) {
    cgh.memcpy(grid->velocity, velocity, sizeof(float) * model_size);
  });
  AcousticDpcComputationParameters::device_queue->wait();
  mem_free(velocity);
  get_suitable_dt(dx, dz, dy, &grid->dt, parameters->second_derivative_fd_coeff,
                  max, parameters->half_length, parameters->dt_relax);
  return grid;
}

void HomogenousModelHandler ::PreprocessModel(
    ComputationKernel *computational_kernel) {

  int nx = grid_box->window_size.window_nx;
  int nz = grid_box->window_size.window_nz;
  int ny = grid_box->window_size.window_ny;

  unsigned int model_size = nx * nz * ny;
  unsigned int bytes = sizeof(float) * model_size;

  // allocating and zeroing prev, curr, and next pressure
  grid_box->pressure_previous = (float *)cl::sycl::malloc_device(
      bytes + 16 * sizeof(float),
      AcousticDpcComputationParameters::device_queue->get_device(),
      AcousticDpcComputationParameters::device_queue->get_context());
  grid_box->pressure_previous =
      &(grid_box->pressure_previous[16 - parameters->half_length]);
  computational_kernel->FirstTouch(grid_box->pressure_previous, nx, nz, ny);
  AcousticDpcComputationParameters::device_queue->submit(
      [&](handler &cgh) { cgh.memset(grid_box->pressure_previous, 0, bytes); });
  grid_box->pressure_current = (float *)cl::sycl::malloc_device(
      bytes + 16 * sizeof(float),
      AcousticDpcComputationParameters::device_queue->get_device(),
      AcousticDpcComputationParameters::device_queue->get_context());
  grid_box->pressure_current =
      &(grid_box->pressure_current[16 - parameters->half_length]);
  computational_kernel->FirstTouch(grid_box->pressure_current, nx, nz, ny);
  AcousticDpcComputationParameters::device_queue->submit(
      [&](handler &cgh) { cgh.memset(grid_box->pressure_current, 0, bytes); });

  grid_box->pressure_next = grid_box->pressure_previous;
  computational_kernel->FirstTouch(grid_box->pressure_next, nx, nz, ny);
  AcousticDpcComputationParameters::device_queue->submit(
      [&](handler &cgh) { cgh.memset(grid_box->pressure_next, 0, bytes); });

  float dt2 = grid_box->dt * grid_box->dt;
  int full_nx = grid_box->grid_size.nx;
  int full_nz = grid_box->grid_size.nz;
  int full_ny = grid_box->grid_size.ny;
  unsigned long full_nx_nz =
      (unsigned long)grid_box->grid_size.nx * grid_box->grid_size.nz;
  // Preprocess the velocity model by calculating the dt2 * c2 component of the
  // wave equation.
  {
    float *vel_device = grid_box->velocity;

    AcousticDpcComputationParameters::device_queue->submit(
        [&](sycl::handler &cgh) {
          auto global_range = range<3>(full_nx, full_ny, full_nz);
          auto local_range = range<3>(1, 1, 1);
          auto global_nd_range = nd_range<3>(global_range, local_range);

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
  }
  AcousticDpcComputationParameters::device_queue->wait();
}

void HomogenousModelHandler ::get_suitable_dt(float dx, float dz, float dy,
                                              float *dt, float *coeff, int max,
                                              int half_length, float dt_relax) {

  // Calculate dt through finite difference stability equation
  float dxSquare = 1 / (dx * dx);
  float dySquare;

  if (dy != 0)
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

void HomogenousModelHandler::SetComputationParameters(
    ComputationParameters *parameters) {
  this->parameters = parameters;
}

void HomogenousModelHandler::SetGridBox(GridBox *grid_box) {
  this->grid_box = (AcousticSecondGrid *)(grid_box);
  if (this->grid_box == nullptr) {
    std::cout << "Not a compatible gridbox : "
                 "expected AcousticSecondGrid"
              << std::endl;
    exit(-1);
  }
}

HomogenousModelHandler ::~HomogenousModelHandler() = default;

void HomogenousModelHandler::SetupWindow() {
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