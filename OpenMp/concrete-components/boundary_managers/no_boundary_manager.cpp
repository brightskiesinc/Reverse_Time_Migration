//
// Created by amrnasr on 21/10/2019.
//

#include "no_boundary_manager.h"
#include "concrete-components/boundary_managers/extensions/zero_extension.h"

// If the constructor is not given any parameters.
NoBoundaryManager::NoBoundaryManager(bool is_staggered) {
  this->extensions.push_back(new ZeroExtension());
  this->is_staggered = is_staggered;
  if (is_staggered) {
    this->extensions.push_back(new ZeroExtension());
  }
}

void NoBoundaryManager::ExtendModel() {
  for (auto const &extension : this->extensions) {
    extension->ExtendProperty();
  }
}

void NoBoundaryManager::ReExtendModel() {
  for (auto const &extension : this->extensions) {
    extension->ReExtendProperty();
  }
}

void NoBoundaryManager::ApplyBoundary(uint kernel_id) {
  // Do nothing for perfect reflection.
}

// The de-constructor.
NoBoundaryManager::~NoBoundaryManager() {
  for (auto const &extension : this->extensions) {
    delete extension;
  }
  this->extensions.clear();
}

void NoBoundaryManager::SetComputationParameters(
    ComputationParameters *parameters) {
  for (auto const &extension : this->extensions) {
    extension->SetHalfLength(parameters->half_length);
    extension->SetBoundaryLength(parameters->boundary_length);
  }
}

void NoBoundaryManager::SetGridBox(GridBox *grid_box) {
  this->extensions[0]->SetGridBox(grid_box);
  this->extensions[0]->SetProperty(grid_box->velocity);
  if (this->is_staggered) {
    StaggeredGrid *grid = (StaggeredGrid *)grid_box;
    this->extensions[1]->SetGridBox(grid);
    this->extensions[1]->SetProperty(grid->density);
  }
}

void NoBoundaryManager::AdjustModelForBackward() {
  for (auto const &extension : this->extensions) {
    extension->AdjustPropertyForBackward();
  }
};
