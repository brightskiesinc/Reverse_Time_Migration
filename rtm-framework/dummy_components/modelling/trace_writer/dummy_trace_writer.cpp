//
// Created by amr on 12/11/2019.
//

#include "dummy_trace_writer.h"
#include <iostream>
// it is just a dummy component it does nothing just print some messages
using namespace std;

DummyTraceWriter::~DummyTraceWriter() {
  cout << "Destroying trace writer" << endl;
}

void DummyTraceWriter::SetComputationParameters(
    ComputationParameters *parameters) {
  cout << "Trace writer : set computation parameters" << endl;
}

void DummyTraceWriter::SetGridBox(GridBox *grid_box) {
  cout << "Trace writer : set grid box" << endl;
}

void DummyTraceWriter::InitializeWriter(
    ModellingConfiguration *modelling_config, string output_filename) {
  cout << "Initializing writer" << endl;
}

void DummyTraceWriter::RecordTrace() { cout << "Recording trace" << endl; }
