//
// Created by amr on 21/10/19.
//

#ifndef ACOUSTIC2ND_RTM_RICKER_SOURCE_INJECTOR_ONEAPI_H
#define ACOUSTIC2ND_RTM_RICKER_SOURCE_INJECTOR_ONEAPI_H

#include <CL/sycl.hpp>
#include <concrete-components/data_units/acoustic_dpc_computation_parameters.h>
#include <concrete-components/data_units/acoustic_second_grid.h>
#include <skeleton/components/source_injector.h>

using namespace cl::sycl;

class RickerSourceInjector : public SourceInjector {
private:
  Point3D *source_point;
  AcousticSecondGrid *grid;
  AcousticDpcComputationParameters *parameters;

public:
  ~RickerSourceInjector() override;

  void ApplySource(uint time_step) override;

  uint GetCutOffTimestep() override;

  void SetComputationParameters(ComputationParameters *parameters) override;

  void SetGridBox(GridBox *grid_box) override;

  void SetSourcePoint(Point3D *source_point) override;
};
#endif // ACOUSTIC2ND_RTM_RICKER_SOURCE_INJECTOR_ONEAPI_H
