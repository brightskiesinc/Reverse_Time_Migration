//
// Created by amrnasr on 21/10/2019.
//

// it is just a dummy component it does nothing just print some messages
#include "dummy_source_injector.h"

#include <iostream>

using namespace std;

void DummySourceInjector::ApplySource(uint time_step) {
  cout << "Injecting source at time : " << time_step << endl;
}

DummySourceInjector::~DummySourceInjector() {
  cout << "Destroying source injector" << endl;
}

void DummySourceInjector::SetComputationParameters(
    ComputationParameters *parameters) {
  cout << "Source injector : set computation parameters" << endl;
}

void DummySourceInjector::SetGridBox(GridBox *grid_box) {
  cout << "Source injector : set grid box" << endl;
}

void DummySourceInjector::SetSourcePoint(Point3D *source_point) {
  cout << "Set source injector source point" << endl;
}

uint DummySourceInjector::GetCutOffTimestep() {
  cout << "Get Cutoff Timestep " << endl;
  return 200;
}
