//
// Created by amrnasr on 21/10/2019.
//

#include "dummy_correlation_kernel.h"
#include <iostream>

using namespace std;

// it is just a dummy component it does nothing just print some messages
void DummyCorrelationKernel::Stack() { cout << "Stacking frames" << endl; }

void DummyCorrelationKernel::Correlate(GridBox *in_1) {
  cout << "Correlating frames" << endl;
}

DummyCorrelationKernel::~DummyCorrelationKernel() {
  cout << "Destroying correlation kernel" << endl;
}

void DummyCorrelationKernel::SetComputationParameters(
    ComputationParameters *parameters) {
  cout << "Correlation kernel : set computation parameters" << endl;
}

void DummyCorrelationKernel::SetGridBox(GridBox *grid_box) {
  cout << "Correlation kernel : set grid box" << endl;
}

void DummyCorrelationKernel::ResetShotCorrelation() {
  cout << "Correlation kernel : reset single shot correlation" << endl;
}

float *DummyCorrelationKernel::GetShotCorrelation() {
  cout << "Correlation kernel : getting shot correlation" << endl;
  return nullptr;
}

float *DummyCorrelationKernel::GetStackedShotCorrelation() {
  cout << "Correlation kernel : getting stacked shot correlation" << endl;
  return nullptr;
}
MigrationData *DummyCorrelationKernel::GetMigrationData() {
  cout << "Correlation kernel : getting final migration data box" << endl;
  return nullptr;
}
