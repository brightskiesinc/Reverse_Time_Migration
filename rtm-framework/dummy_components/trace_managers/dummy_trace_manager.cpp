//
// Created by amrnasr on 21/10/2019.
//
// it is just a dummy component it does nothing just print some messages
#include "dummy_trace_manager.h"

#include <iostream>

using namespace std;

void DummyTraceManager::ApplyTraces(uint time_step) {
  cout << "Applying traces for time-step " << time_step << endl;
}

void DummyTraceManager::PreprocessShot(uint cut_off_time) {
  cout << "Pre-processing shot " << endl;
}

void DummyTraceManager::ReadShot(vector<string> filenames, uint shot_number, string sort_key) {
  cout << "Reading shot num #" << shot_number << " with sort key " << sort_key << endl;
  this->dummy_grid->nt = 25;
  source_point = {this->dummy_grid->window_size.window_nx / 2, 24, 0};
}

DummyTraceManager::~DummyTraceManager() {
  cout << "Destroying trace manager" << endl;
}

Traces *DummyTraceManager::GetTraces() {
  cout << "Getting traces" << endl;
  return nullptr;
}

void DummyTraceManager::SetComputationParameters(
    ComputationParameters *parameters) {
  cout << "Trace manager : set computation parameters" << endl;
}

void DummyTraceManager::SetGridBox(GridBox *grid_box) {
  cout << "Trace manager : set grid box" << endl;
  this->dummy_grid = grid_box;
}

Point3D *DummyTraceManager::GetSourcePoint() {
  cout << "Get source point from shot" << endl;
  return &source_point;
}

vector<uint> DummyTraceManager::GetWorkingShots(vector<string> filenames, uint min_shot, uint max_shot, string type) {
    vector<uint> shots;
    cout << "Getting all possible shots from the trace manager" << endl;
    for (uint i = min_shot; i <= max_shot; i++) {
        shots.push_back(i);
    }
    return shots;
}

