//
// Created by amrnasr on 12/11/2019.
//
// This should contain the main function that drives the RTM acoustic 2nd order
// wave equation execution.
#include <cmd_parser_utils.h>
#include <parsers/callback_parser.h>
#include <parsers/configuration_parser.h>
#include <parsers/parameter_parser.h>
#include <skeleton/base/engine_configuration.h>
#include <skeleton/engine/modelling_engine.h>

int main(int argc, char *argv[]) {
  string parameter_file = WORKLOAD_PATH "/computation_parameters.txt";
  string configuration_file = WORKLOAD_PATH "/modelling_configuration.txt";
  string callback_file = WORKLOAD_PATH "/callback_configuration.txt";
  string write_path = WRITE_PATH;
  string message = "perform wavefield modelling";
  parse_args_engine(parameter_file, configuration_file, callback_file,
                    write_path, argc, argv, message.c_str());
  ComputationParameters *p = ParseParameterFile(parameter_file);
  ModellingEngineConfiguration *modelling_configuration =
      parse_modelling_configuration(configuration_file, write_path);
  CallbackCollection *cbs = parse_callbacks(callback_file, write_path);
  ModellingEngine engine(modelling_configuration, p, cbs);
  engine.Model();
  cout <<endl<<"Timings of the application are: "<<endl;
  cout <<"------------------------------"<<endl;
  Timer *timer = Timer::getInstance();
  timer->export_to_file(write_path + "/timing_results.txt",1);
  delete modelling_configuration;
  delete cbs;
  delete p;
  return 0;
}
