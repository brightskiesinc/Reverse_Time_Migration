//
// Created by amr on 18/11/2019.
//

#ifndef ACOUSTIC2ND_RTM_RANDOM_BOUNDARY_MANAGER_H
#define ACOUSTIC2ND_RTM_RANDOM_BOUNDARY_MANAGER_H

#include "concrete-components/boundary_managers/extensions/extension.h"
#include <concrete-components/data_units/staggered_grid.h>
#include <skeleton/components/boundary_manager.h>
#include <vector>
/*!
 * Random boundary manager : based on
 * https://library.seg.org/doi/abs/10.1190/1.3255432.
 */
class RandomBoundaryManager : public BoundaryManager {
private:
  std::vector<Extension *> extensions;
  bool is_staggered;

public:
  // Constructor.
  RandomBoundaryManager(bool is_staggered = false);
  // De-constructor.
  ~RandomBoundaryManager() override;
  // Will do nothing.
  void ApplyBoundary(uint kernel_id = 0) override;
  // Extend the velocities at boundaries randomly.
  void ExtendModel() override;
  // In case of window mode extend the velocities at boundaries randomly.
  void ReExtendModel() override;

  void SetComputationParameters(ComputationParameters *parameters) override;

  void SetGridBox(GridBox *grid_box) override;

  void AdjustModelForBackward() override;
};

#endif // ACOUSTIC2ND_RTM_RANDOM_BOUNDARY_MANAGER_H
