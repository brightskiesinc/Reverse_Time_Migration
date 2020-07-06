#include "ricker_source_injector.h"
#include <concrete-components/acoustic_second_components.h>
#include <iostream>
#include <math.h>

/**
 * Implementation based on
 * https://tel.archives-ouvertes.fr/tel-00954506v2/document .
 */
void RickerSourceInjector::ApplySource(uint time_step) {
  float dt = grid->dt;
  float freq = parameters->source_frequency;
  int x = source_point->x;
  int y = source_point->y;
  int z = source_point->z;
  int nx = grid->window_size.window_nx;
  int ny = grid->window_size.window_ny;
  int nz = grid->window_size.window_nz;
  if (time_step < this->GetCutOffTimestep()) {
    {
      float temp = M_PI * M_PI * freq * freq *
                   (((time_step - 1) * dt) - 1 / freq) *
                   (((time_step - 1) * dt) - 1 / freq);
      float ricker = (2 * temp - 1) * exp(-temp);
      ricker = ricker * dt * dt;
      int location = y * nx * nz + z * nx + x;
      AcousticDpcComputationParameters::device_queue->submit([&](handler &cgh) {
        auto pressure = grid->pressure_current;
        cgh.single_task<class source_injector>(
            [=]() { pressure[location] += ricker; });
      });
      AcousticDpcComputationParameters::device_queue->wait();
    }
  }
}

uint RickerSourceInjector::GetCutOffTimestep() {
  float dt = grid->dt;
  float freq = parameters->source_frequency;
  return (2.0 / freq) / dt;
}

void RickerSourceInjector::SetComputationParameters(
    ComputationParameters *parameters) {
  this->parameters = (AcousticDpcComputationParameters *)(parameters);
  if (this->parameters == nullptr) {
    std::cout << "Not a compatible computation parameters : "
                 "expected AcousticDpcComputationParameters"
              << std::endl;
    exit(-1);
  }
}

void RickerSourceInjector::SetGridBox(GridBox *grid_box) {
  this->grid = (AcousticSecondGrid *)(grid_box);
  if (this->grid == nullptr) {
    std::cout << "Not a compatible gridbox : "
                 "expected AcousticSecondGrid"
              << std::endl;
    exit(-1);
  }
}

void RickerSourceInjector::SetSourcePoint(Point3D *source_point) {
  this->source_point = source_point;
}

RickerSourceInjector::~RickerSourceInjector() = default;
