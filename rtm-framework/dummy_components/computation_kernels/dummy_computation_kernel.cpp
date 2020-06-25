//
// Created by amrnasr on 21/10/2019.
//

#include "dummy_computation_kernel.h"

#include <iostream>

// it is just a dummy component it does nothing just print some messages
using namespace std;

void DummyComputationKernel::Step() {
  cout << "Stepping through time" << endl;
  this->boundary_manager->ApplyBoundary();
}

void DummyComputationKernel::FirstTouch(float *ptr, uint nx, uint nz, uint ny) {
  cout << "Doing first touch" << endl;
}

DummyComputationKernel::~DummyComputationKernel() {
  cout << "Destroying the computational kernel" << endl;
}

void DummyComputationKernel::SetComputationParameters(
    ComputationParameters *parameters) {
  cout << "Computation kernel : set computation parameters" << endl;
}

void DummyComputationKernel::SetGridBox(GridBox *grid_box) {
  cout << "Computation kernel : set grid box" << endl;
}
