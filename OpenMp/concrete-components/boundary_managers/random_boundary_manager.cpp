//
// Created by amr on 18/11/2019.
//

#include "random_boundary_manager.h"
#include "extensions/min_extension.h"
#include "extensions/random_extension.h"
#include <cstdlib>
#include <ctime>

// If the constructor is not given any parameters.
RandomBoundaryManager::RandomBoundaryManager(bool is_staggered) {
  this->extensions.push_back(new RandomExtension());
  this->is_staggered = is_staggered;
  if (is_staggered) {
    // Extend density with minimum value to ensure stability with the randomized
    // velocity.
    this->extensions.push_back(new MinExtension());
  }
  srand(time(NULL));
}

void RandomBoundaryManager::ExtendModel() {
  for (auto const &extension : this->extensions) {
    extension->ExtendProperty();
  }
}

void RandomBoundaryManager::ReExtendModel() {
  for (auto const &extension : this->extensions) {
    extension->ExtendProperty();
    extension->ReExtendProperty();
  }
}

void RandomBoundaryManager::ApplyBoundary(uint kernel_id) {
  // Do nothing for random boundaries.
}

// The de-constructor.
RandomBoundaryManager::~RandomBoundaryManager() {
  for (auto const &extension : this->extensions) {
    delete extension;
  }
  this->extensions.clear();
}

void RandomBoundaryManager::SetComputationParameters(
    ComputationParameters *parameters) {
  for (auto const &extension : this->extensions) {
    extension->SetHalfLength(parameters->half_length);
    extension->SetBoundaryLength(parameters->boundary_length);
  }
}

void RandomBoundaryManager::SetGridBox(GridBox *grid_box) {
  this->extensions[0]->SetGridBox(grid_box);
  this->extensions[0]->SetProperty(grid_box->velocity);
  if (this->is_staggered) {
    StaggeredGrid *grid = (StaggeredGrid *)grid_box;
    this->extensions[1]->SetGridBox(grid);
    this->extensions[1]->SetProperty(grid->density);
  }
}

void RandomBoundaryManager::AdjustModelForBackward() {
  for (auto const &extension : this->extensions) {
    extension->AdjustPropertyForBackward();
  }
};