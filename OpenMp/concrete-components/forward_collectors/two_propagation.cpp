#include "two_propagation.h"
#include <compress.h>
#include <iostream>
#include <sys/stat.h>

TwoPropagation::TwoPropagation(bool compression, string write_path,
                               float zfp_tolerance, int zfp_parallel,
                               bool zfp_is_relative) {
  this->internal_grid = (AcousticSecondGrid *)mem_allocate(
      sizeof(AcousticSecondGrid), 1, "forward_collector_gridbox");
  this->internal_grid->pressure_current = nullptr;
  this->forward_pressure = nullptr;
  mem_fit = false;
  time_counter = 0;
  mkdir(write_path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  this->write_path = write_path + "/two_prop";
  mkdir(this->write_path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  zfp::setPath(this->write_path);
  this->compression = compression;
  this->zfp_tolerance = zfp_tolerance;
  this->zfp_parallel = zfp_parallel + 1;
  this->zfp_is_relative = zfp_is_relative;
}
void TwoPropagation::FetchForward(void) {
  if (mem_fit) {
    internal_grid->pressure_current =
        internal_grid->pressure_current - pressure_size;
  } else {
    if ((time_counter + 1) % max_nt == 0) {
      if (this->compression) {
        for (int it = 0; it < max_nt; it++) {
          size_t resultSize;
          zfp::decompression(&forward_pressure[it * pressure_size],
                             main_grid->window_size.window_nx,
                             main_grid->window_size.window_ny,
                             main_grid->window_size.window_nz,
                             (double)this->zfp_tolerance, this->zfp_parallel,
                             (unsigned int)(time_counter + 1 - max_nt + it),
                             &resultSize, "forward_pressure",
                             this->zfp_is_relative);
        }
      } else {
        string s =
            this->write_path + "/temp_" + to_string(time_counter / max_nt);
        bin_file_load(s.c_str(), forward_pressure, max_nt * pressure_size);
      }
    }
    internal_grid->pressure_current =
        forward_pressure + ((time_counter) % max_nt) * pressure_size;
    time_counter--;
  }
}

void TwoPropagation::ResetGrid(bool forward_run) {
  if (forward_run) {
    pressure_size = main_grid->window_size.window_nx *
                    main_grid->window_size.window_ny *
                    main_grid->window_size.window_nz;
    time_counter = 0;
    if (forward_pressure == nullptr) {
      // Add one for empty timeframe at the start of the simulation(The first
      // previous) since SaveForward is called before each step.
      max_nt = main_grid->nt + 1;
      forward_pressure = (float *)mem_allocate(
          (sizeof(float)), max_nt * pressure_size, "forward_pressure");
      if (forward_pressure != nullptr) {
        mem_fit = true;
      } else {
        mem_fit = false;
        while (forward_pressure == nullptr) {
          max_nt = max_nt / 2;
          forward_pressure = (float *)mem_allocate(
              (sizeof(float)), max_nt * pressure_size, "forward_pressure");
        }
      }
    }
    temp_prev = main_grid->pressure_previous;
    temp_curr = main_grid->pressure_current;
    temp_next = main_grid->pressure_next;
    memset(forward_pressure, 0.0f, pressure_size * sizeof(float));
    memset(forward_pressure + pressure_size, 0.0f,
           pressure_size * sizeof(float));
    memset(forward_pressure + 2 * pressure_size, 0.0f,
           pressure_size * sizeof(float));
    main_grid->pressure_previous = forward_pressure;
    main_grid->pressure_current = forward_pressure + pressure_size;
    // Save forward is called before the kernel in the engine.
    // When called will advance pressure next to the right point.
    main_grid->pressure_next = forward_pressure + pressure_size;
    internal_grid->nt = main_grid->nt;
    internal_grid->dt = main_grid->dt;
    memcpy(&internal_grid->grid_size, &main_grid->grid_size,
           sizeof(main_grid->grid_size));
    memcpy(&internal_grid->window_size, &main_grid->window_size,
           sizeof(main_grid->window_size));
    memcpy(&internal_grid->cell_dimensions, &main_grid->cell_dimensions,
           sizeof(main_grid->cell_dimensions));
    internal_grid->velocity = main_grid->velocity;
    internal_grid->window_velocity = main_grid->window_velocity;
  } else {
    memset(temp_prev, 0.0f, pressure_size * sizeof(float));
    memset(temp_curr, 0.0f, pressure_size * sizeof(float));
    if (!mem_fit) {
      time_counter++;
      internal_grid->pressure_current = main_grid->pressure_current;
    } else {
      // Pressure size will be minimized in FetchForward call at first step.
      internal_grid->pressure_current =
          main_grid->pressure_current + pressure_size;
    }
    main_grid->pressure_previous = temp_prev;
    main_grid->pressure_current = temp_curr;
    main_grid->pressure_next = temp_next;
  }
}
void TwoPropagation::SaveForward() {
  if (mem_fit) {
    main_grid->pressure_next = main_grid->pressure_current + pressure_size;
  } else {
    time_counter++;
    if ((time_counter + 1) % max_nt == 0) {
      if (this->compression) {
        for (int it = 0; it < max_nt; it++) {
          size_t resultSize;
          zfp::compression(forward_pressure + it * pressure_size,
                           main_grid->window_size.window_nx,
                           main_grid->window_size.window_ny,
                           main_grid->window_size.window_nz,
                           (double)this->zfp_tolerance, this->zfp_parallel,
                           (unsigned int)(time_counter + 1 - max_nt + it),
                           &resultSize, "forward_pressure",
                           this->zfp_is_relative);
        }
      } else {
        string s =
            this->write_path + "/temp_" + to_string(time_counter / max_nt);
        bin_file_save(s.c_str(), forward_pressure, max_nt * pressure_size);
      }
    }
    main_grid->pressure_previous =
        forward_pressure + ((time_counter - 1) % max_nt) * pressure_size;
    main_grid->pressure_current =
        forward_pressure + ((time_counter) % max_nt) * pressure_size;
    main_grid->pressure_next =
        forward_pressure + ((time_counter + 1) % max_nt) * pressure_size;
  }
}
TwoPropagation::~TwoPropagation() {
  if (forward_pressure != NULL) {
    mem_free((void *)forward_pressure);
  }
  mem_free((void *)internal_grid);
}

void TwoPropagation::SetComputationParameters(
    ComputationParameters *parameters) {
  this->parameters = parameters;
}

void TwoPropagation::SetGridBox(GridBox *grid_box) {
  this->main_grid = (AcousticSecondGrid *)(grid_box);
  if (this->main_grid == nullptr) {
    std::cout << "Not a compatible gridbox : "
                 "expected AcousticSecondGrid"
              << std::endl;
    exit(-1);
  }
}

GridBox *TwoPropagation::GetForwardGrid() { return internal_grid; }
