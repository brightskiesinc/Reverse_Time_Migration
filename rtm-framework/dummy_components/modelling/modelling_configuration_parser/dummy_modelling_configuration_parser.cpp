//
// Created by amr on 12/11/2019.
//

#include "dummy_modelling_configuration_parser.h"
#include <iostream>

using namespace std;
// it is just a dummy component it does nothing just print some messages
DummyModellingConfigurationParser::~DummyModellingConfigurationParser() {
  cout << "Destroying modelling configuration parser" << endl;
}

ModellingConfiguration
DummyModellingConfigurationParser::ParseConfiguration(string filepath,
                                                      bool is_2D) {
  ModellingConfiguration conf;
  conf.total_time = 1;
  conf.source_point = {0, 0, 0};
  conf.receivers_start = {0, 0, 0};
  conf.receivers_increment = {1, 0, 0};
  conf.receivers_end = {
      1,
      1,
      1,
  };
  cout << "Parsing modelling configuration" << endl;
  return conf;
}

void DummyModellingConfigurationParser::SetComputationParameters(
    ComputationParameters *parameters) {
  cout << "Modelling Configuration Parser : set computation parameters" << endl;
}

void DummyModellingConfigurationParser::SetGridBox(GridBox *grid_box) {
  cout << "Modelling Configuration Parser : set grid box" << endl;
}
