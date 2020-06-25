//
// Created by amrnasr on 21/10/2019.
//

#include "dummy_forward_collector.h"

#include <iostream>

// it is just a dummy component it does nothing just print some messages
using namespace std;

void DummyForwardCollector::SaveForward() {
  cout << "Saving forward frame" << endl;
}

void DummyForwardCollector::FetchForward() {
  cout << "Fetching forward frame" << endl;
}

void DummyForwardCollector::ResetGrid(bool forward_run) {
  cout << "Resetting grid : is forward run = " << forward_run << endl;
}

DummyForwardCollector::~DummyForwardCollector() {
  cout << "Destroying forward collector" << endl;
}

void DummyForwardCollector::SetComputationParameters(
    ComputationParameters *parameters) {
  cout << "Forward collector : set computation parameters" << endl;
}

void DummyForwardCollector::SetGridBox(GridBox *grid_box) {
  cout << "Forward collector : set grid box" << endl;
}

GridBox *DummyForwardCollector::GetForwardGrid() {
  cout << "Get forward grid" << endl;
  return nullptr;
}
