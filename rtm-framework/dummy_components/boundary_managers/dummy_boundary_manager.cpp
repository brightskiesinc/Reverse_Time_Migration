//
// Created by amrnasr on 21/10/2019.
//

// it is just a dummy component it does nothing just print some messages
#include "dummy_boundary_manager.h"
#include <iostream>

using namespace std;

void DummyBoundaryManager::ExtendModel() {
  cout << "Extending the model velocities" << endl;
}

void DummyBoundaryManager::ReExtendModel() {
  cout << "Re-extending the model velocities" << endl;
}

void DummyBoundaryManager::ApplyBoundary(uint kernel_id) {
  cout << "Apply boundaries " << endl;
}

DummyBoundaryManager::~DummyBoundaryManager() {
  cout << "Destroying the boundary manager" << endl;
}

void DummyBoundaryManager::SetComputationParameters(
    ComputationParameters *parameters) {
  cout << "Boundary manager : Set computation parameters" << endl;
}

void DummyBoundaryManager::SetGridBox(GridBox *grid_box) {
  cout << "Boundary manager : Set grid box" << endl;
}

void DummyBoundaryManager::AdjustModelForBackward() {
  cout << "Adjust model in backward for boundary conditions  " << endl;
}
