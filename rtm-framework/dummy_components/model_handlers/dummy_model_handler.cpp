//
// Created by amrnasr on 21/10/2019.
//

#include "dummy_model_handler.h"
#include <skeleton/helpers/memory_allocation/memory_allocator.h>

#include <iostream>

// it is just a dummy component it does nothing just print some messages and
// setting some values
using namespace std;

GridBox *DummyModelHandler::ReadModel(vector<string> filenames,
                                      ComputationKernel *kernel) {
  cout << "Reading model" << endl;
  GridBox *temp = (GridBox *)mem_allocate(sizeof(GridBox), 1, "GridBox");
  temp->grid_size.nx = 1;
  temp->grid_size.ny = 2;
  temp->grid_size.nz = 3;
  temp->dt = 0.1;
  return temp;
}

void DummyModelHandler::PreprocessModel(ComputationKernel *kernel) {
  cout << "Pre-processing model" << endl;
}

DummyModelHandler::~DummyModelHandler() {
  cout << "Destroying model handler" << endl;
}

void DummyModelHandler::SetComputationParameters(
    ComputationParameters *parameters) {
  cout << "Model handler : set computation parameters" << endl;
}

void DummyModelHandler::SetGridBox(GridBox *grid_box) {
  cout << "Model handler : set grid box" << endl;
}
