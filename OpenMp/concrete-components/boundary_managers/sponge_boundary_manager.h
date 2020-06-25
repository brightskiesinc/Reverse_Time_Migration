//
// Created by mirnamoawad on 11/21/19.
//

#ifndef ACOUSTIC2ND_RTM_SPONGE_BOUNDARY_MANAGER_H
#define ACOUSTIC2ND_RTM_SPONGE_BOUNDARY_MANAGER_H

#include <concrete-components/boundary_managers/extensions/extension.h>
#include <concrete-components/data_units/staggered_grid.h>
#include <rtm-framework/skeleton/components/boundary_manager.h>
#include <vector>

class SpongeBoundaryManager : public BoundaryManager {
private:
  std::vector<Extension *> extensions;

  bool is_staggered;

  float *sponge_coeffs;

  float calculation(int index);

  ComputationParameters *parameters;

  GridBox *grid;

  void ApplyBoundaryOnField(float *next);

public:
  SpongeBoundaryManager(bool use_top_layer = true, bool is_staggered = false);

  ~SpongeBoundaryManager() override;

  void ApplyBoundary(uint kernel_id = 0) override;

  void ExtendModel() override;

  void ReExtendModel() override;

  void SetComputationParameters(ComputationParameters *parameters) override;

  void SetGridBox(GridBox *grid_box) override;

  void AdjustModelForBackward() override;
};

#endif // ACOUSTIC2ND_RTM_SPONGE_BOUNDARY_MANAGER_H
