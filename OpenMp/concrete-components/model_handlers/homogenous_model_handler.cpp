#include "homogenous_model_handler.h"
#include <cmath>
#include <concrete-components/data_units/acoustic_second_grid.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

float HomogenousModelHandler::SetModelField(float *field, vector<float> &val,
                                            int nx, int nz, int ny) {
  // extracting Velocity and size of each layer in terms of start(x,y,z) and
  // end(x,y,z)
  float vel;
  int vsx, vsy, vsz, vex, vey, vez;
  float max = 0;
  for (int i = 0; i < val.size(); i += 7) {
    vel = val[i];
    if (vel > max) {
      max = vel;
    }
    // starting filling the velocity with required value value after shifting
    // the padding of boundaries and half_length
    int offset = parameters->boundary_length + parameters->half_length;
    vsx = (int)val[i + 1] + offset;
    vsz = (int)val[i + 2] + offset;
    vsy = (int)val[i + 3];
    vex = (int)val[i + 4] + offset;
    vez = (int)val[i + 5] + offset;
    vey = (int)val[i + 6];

    if (ny > 1) {
      vsy = (int)vsy + offset;
      vey = (int)vey + offset;
    }
    if (vsx < 0 || vsy < 0 || vsz < 0) {
      cout << "Error at starting index values (x,y,z) (" << vsx << "," << vsy
           << "," << vsz << ")" << endl;
      continue;
    }
    if (vex > nx || vey > ny || vez > nz) {
      cout << "Error at ending index values (x,y,z) (" << vex << "," << vey
           << "," << vez << ")" << endl;
      continue;
    }

    // initialize the layer with the extracted velocity value
#pragma omp parallel default(shared)
    {
#pragma omp for schedule(static) collapse(3)
      for (int y = vsy; y < vey; y++) {
        for (int z = vsz; z < vez; z++) {
          for (int x = vsx; x < vex; x++) {
            field[y * nx * nz + z * nx + x] = vel;
          }
        }
      }
    }
  }
  return max;
}

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
  GridBox *grid;
  if (is_staggered) {
    grid = (GridBox *)mem_allocate(sizeof(StaggeredGrid), 1, "StaggeredGrid");
  } else {
    grid = (GridBox *)mem_allocate(sizeof(AcousticSecondGrid), 1, "GridBox");
  }
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

  float dx, dy, dz, dt;

  dx = grid->cell_dimensions.dx = val[0][3];
  dz = grid->cell_dimensions.dz = val[0][4];
  dy = grid->cell_dimensions.dy = val[0][5];
  // Calculate Model Size
  unsigned int model_size = nx * nz * ny;

  // if there is no window then window size equals full model size
#ifndef WINDOW_MODEL
  grid->window_size.window_start.x = 0;
  grid->window_size.window_start.z = 0;
  grid->window_size.window_start.y = 0;
  grid->window_size.window_nx = nx;
  grid->window_size.window_nz = nz;
  grid->window_size.window_ny = ny;
#else
  grid->window_size.window_start.x = val[2][0];
  grid->window_size.window_start.z = val[2][1];
  grid->window_size.window_start.y = val[2][2];
  grid->window_size.window_nx = val[2][3];
  grid->window_size.window_nz = val[2][4];
  grid->window_size.window_ny = val[2][5];
#endif

  // Allocation and Zeroing the velocity model
  float *velocity = (float *)mem_allocate(sizeof(float), model_size, "velocity",
                                          parameters->half_length, 0);
  computational_kernel->FirstTouch(velocity, grid->grid_size.nx,
                                   grid->grid_size.nz, grid->grid_size.ny);
  memset(velocity, 0, sizeof(float) * model_size);

  // extracting Velocity and size of each layer in terms of start(x,y,z) and
  // end(x,y,z)
  float max = this->SetModelField(velocity, val[1], nx, nz, ny);

  grid->velocity = velocity;

  if (is_staggered) {
    StaggeredGrid *s_grid = (StaggeredGrid *)grid;
    float *density = (float *)mem_allocate(sizeof(float), model_size, "density",
                                           parameters->half_length, 0);
    computational_kernel->FirstTouch(density, grid->grid_size.nx,
                                     grid->grid_size.nz, grid->grid_size.ny);
    memset(density, 0, sizeof(float) * model_size);
    this->SetModelField(density, val[3], nx, nz, ny);
    s_grid->density = density;
  }

  GetSuitableDt(dx, dz, dy, &grid->dt, parameters->second_derivative_fd_coeff,
                max, parameters->half_length, parameters->dt_relax);
  return grid;
}

void HomogenousModelHandler ::PreprocessModel(
    ComputationKernel *computational_kernel) {
  int nx = grid_box->window_size.window_nx;
  int nz = grid_box->window_size.window_nz;
  int ny = grid_box->window_size.window_ny;

  unsigned int model_size = nx * nz * ny;
  // allocating and zeroing prev, curr, and next pressure
  float *curr = (float *)mem_allocate(sizeof(float), model_size, "curr",
                                      parameters->half_length, 32);
  grid_box->pressure_current = curr;
  computational_kernel->FirstTouch(curr, nx, nz, ny);
  memset(curr, 0, sizeof(float) * model_size);

  if (is_staggered) {
    StaggeredGrid *grid_box = (StaggeredGrid *)this->grid_box;
    float *particle_vel_x =
        (float *)mem_allocate(sizeof(float), model_size, "particle_vel_x",
                              parameters->half_length, 16);
    grid_box->particle_velocity_x_current = particle_vel_x;
    computational_kernel->FirstTouch(particle_vel_x, nx, nz, ny);
    memset(particle_vel_x, 0, sizeof(float) * model_size);

    float *particle_vel_z =
        (float *)mem_allocate(sizeof(float), model_size, "particle_vel_z",
                              parameters->half_length, 48);
    grid_box->particle_velocity_z_current = particle_vel_z;
    computational_kernel->FirstTouch(particle_vel_x, nx, nz, ny);
    memset(particle_vel_x, 0, sizeof(float) * model_size);

    if (ny > 1) {
      float *particle_vel_y =
          (float *)mem_allocate(sizeof(float), model_size, "particle_vel_y",
                                parameters->half_length, 64);
      grid_box->particle_velocity_y_current = particle_vel_y;
      computational_kernel->FirstTouch(particle_vel_y, nx, nz, ny);
      memset(particle_vel_y, 0, sizeof(float) * model_size);
    }

    float *next = grid_box->pressure_current;
    grid_box->pressure_next = next;
    computational_kernel->FirstTouch(next, nx, nz, ny);
    memset(next, 0, sizeof(float) * model_size);
  } else {
    AcousticSecondGrid *grid_box = (AcousticSecondGrid *)this->grid_box;
    float *prev = (float *)mem_allocate(sizeof(float), model_size, "prev",
                                        parameters->half_length, 16);
    grid_box->pressure_previous = prev;
    computational_kernel->FirstTouch(prev, nx, nz, ny);
    memset(prev, 0, sizeof(float) * model_size);

    float *next = prev;
    grid_box->pressure_next = next;
    computational_kernel->FirstTouch(next, nx, nz, ny);
    memset(next, 0, sizeof(float) * model_size);
  }
  float dt = grid_box->dt;
  float dt2 = grid_box->dt * grid_box->dt;
  float *velocity_values = grid_box->velocity;
  int full_nx = grid_box->grid_size.nx;
  int full_nx_nz = grid_box->grid_size.nx * grid_box->grid_size.nz;
  if (is_staggered) {
    StaggeredGrid *grid_box = (StaggeredGrid *)this->grid_box;
    float *density_values = grid_box->density;
// Preprocess the velocity model by calculating the dt * c2 * density component
// of the wave equation.
#pragma omp parallel default(shared)
    {
#pragma omp for schedule(static) collapse(3)
      for (int y = 0; y < grid_box->grid_size.ny; ++y) {
        for (int z = 0; z < grid_box->grid_size.nz; ++z) {
          for (int x = 0; x < grid_box->grid_size.nx; ++x) {
            float value = velocity_values[y * full_nx_nz + z * full_nx + x];
            int offset = y * full_nx_nz + z * full_nx + x;
            velocity_values[offset] =
                value * value * dt * density_values[offset];
            if (density_values[offset] != 0) {
              density_values[offset] = dt / density_values[offset];
            }
          }
        }
      }
    }
  } else {
// Preprocess the velocity model by calculating the dt2 * c2 component of the
// wave equation.
#pragma omp parallel default(shared)
    {
#pragma omp for schedule(static) collapse(3)
      for (int y = 0; y < grid_box->grid_size.ny; ++y) {
        for (int z = 0; z < grid_box->grid_size.nz; ++z) {
          for (int x = 0; x < grid_box->grid_size.nx; ++x) {
            float value = velocity_values[y * full_nx_nz + z * full_nx + x];
            velocity_values[y * full_nx_nz + z * full_nx + x] =
                value * value * dt2;
          }
        }
      }
    }
  }
}

void HomogenousModelHandler ::GetSuitableDt(float dx, float dz, float dy,
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
  this->grid_box = grid_box;
}

HomogenousModelHandler::HomogenousModelHandler(bool staggered) {
  this->is_staggered = staggered;
}

HomogenousModelHandler ::~HomogenousModelHandler() = default;
