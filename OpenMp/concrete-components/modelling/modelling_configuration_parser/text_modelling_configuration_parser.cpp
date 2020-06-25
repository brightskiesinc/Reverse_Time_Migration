//
// Created by amrnasr on 13/11/2019.
//

#include "text_modelling_configuration_parser.h"

#include <fstream>
#include <iostream>
#include <string>

using namespace std;

void PrintPoint(Point3D *point) {
  cout << "(x = " << point->x << ", z = " << point->z << ", y = " << point->y
       << ")" << endl;
}

void PrintModellingConfiguration(ModellingConfiguration *conf) {
  cout << "Simulation time : " << conf->total_time << endl;
  cout << "Source point : ";
  PrintPoint(&conf->source_point);
  cout << "Receivers start point : ";
  PrintPoint(&conf->receivers_start);
  cout << "Receivers increment values : ";
  PrintPoint(&conf->receivers_increment);
  cout << "Receivers end point : ";
  PrintPoint(&conf->receivers_end);
}

TextModellingConfigurationParser::~TextModellingConfigurationParser() {}

Point3D ParseStringPoint(string s) {
  Point3D point;
  string temp = s.substr(1, s.size() - 1);
  string x = temp.substr(0, temp.find(","));
  string zy = temp.substr(temp.find(",") + 1);
  string z = zy.substr(0, zy.find(","));
  string y = zy.substr(zy.find(",") + 1);
  point.x = atoi(x.c_str());
  point.z = atoi(z.c_str());
  point.y = atoi(y.c_str());
  return point;
}

ModellingConfiguration
TextModellingConfigurationParser::ParseConfiguration(string filepath,
                                                     bool is_2D) {
  ifstream model_file(filepath);
  if (!model_file.is_open()) {
      cout << "Couldn't open modelling parameters file '" << filepath << "'..." << std::endl;
      exit(0);
  }
  ModellingConfiguration conf;
  string temp_line;
  uint half_length = this->parameters->half_length;
  uint bound_length = this->parameters->boundary_length;
  while (model_file >> temp_line) {
    string key = temp_line.substr(0, temp_line.find("="));
    string value = temp_line.substr(temp_line.find("=") + 1);
    if (key == "source") {
      conf.source_point = ParseStringPoint(value);
      conf.source_point.x += half_length + bound_length;
      if (!is_2D) {
        conf.source_point.y += half_length + bound_length;
      } else {
        conf.source_point.y = 0;
      }
      conf.source_point.z += half_length + bound_length;
    } else if (key == "receiver_start") {
      conf.receivers_start = ParseStringPoint(value);
      conf.receivers_start.x += half_length + bound_length;
      if (!is_2D) {
        conf.receivers_start.y += half_length + bound_length;
      } else {
        conf.receivers_start.y = 0;
      }
      conf.receivers_start.z += half_length + bound_length;
    } else if (key == "receiver_inc") {
      conf.receivers_increment = ParseStringPoint(value);
    } else if (key == "receiver_end") {
      conf.receivers_end = ParseStringPoint(value);
      conf.receivers_end.x += half_length + bound_length;
      if (!is_2D) {
        conf.receivers_end.y += half_length + bound_length;
      } else {
        conf.receivers_end.y = 1;
      }
      conf.receivers_end.z += half_length + bound_length;
    } else if (key == "simulation_time") {
      conf.total_time = atof(value.c_str());
    }
  }
  PrintModellingConfiguration(&conf);
  return conf;
}

void TextModellingConfigurationParser::SetComputationParameters(
    ComputationParameters *parameters) {
  this->parameters = (AcousticOmpComputationParameters *)(parameters);
  if (this->parameters == nullptr) {
    std::cout << "Not a compatible computation parameters : "
                 "expected AcousticOmpComputationParameters"
              << std::endl;
    exit(-1);
  }
}

void TextModellingConfigurationParser::SetGridBox(GridBox *grid_box) {
  // Not needed.
}
