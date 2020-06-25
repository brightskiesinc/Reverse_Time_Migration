//
// Created by amrnasr on 21/10/2019.
//

#ifndef RTM_FRAMEWORK_DUMMY_BOUNDARY_MANAGER_H
#define RTM_FRAMEWORK_DUMMY_BOUNDARY_MANAGER_H

#include "concrete-components/boundary_managers/extensions/extension.h"
#include <skeleton/components/boundary_manager.h>
#include <vector>

// the NoBoundaryManager  class inheritance from the public BoundaryManager
// class
class NoBoundaryManager : public BoundaryManager {
private:
  std::vector<Extension *> extensions;
  bool is_staggered;

public:
  // Constructor
  NoBoundaryManager(bool is_staggered = false);
  // Deconstructor
  ~NoBoundaryManager() override;

  void ApplyBoundary(uint kernel_id = 0) override;
  // extend the velocities at boundaries by zeros
  void ExtendModel() override;
  // in case of window mode extend the velocities at boundaries by zeros
  void ReExtendModel() override;

  void SetComputationParameters(ComputationParameters *parameters) override;

  void SetGridBox(GridBox *grid_box) override;

  void AdjustModelForBackward() override;
};

#endif // RTM_FRAMEWORK_DUMMY_BOUNDARY_MANAGER_H
